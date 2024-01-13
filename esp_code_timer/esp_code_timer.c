/**
 * @file code_timer.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-06-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "esp_code_timer.h"

#include <string.h>

#include "esp_timer.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define PRINT_QUEUE_LENGTH 5

static const char *TAG = "esp_code_timer";
static bool timers_active = true;
static uint32_t last_timestamp = 0;
static float average = 0;
static size_t n = 0;

static QueueHandle_t print_queue = NULL;
static TaskHandle_t print_task = NULL;

void esp_code_timer_start(void)
{
    last_timestamp = (uint32_t)esp_timer_get_time();
}

uint32_t esp_code_timer_stop(void)
{
    uint32_t now = (uint32_t)esp_timer_get_time();

    if (last_timestamp == 0) {
        ESP_LOGE(TAG, "code_timer_sh_start() must be called before code_timer_sh_stop()");
        return 0;
    }

    uint32_t delta = now - last_timestamp;

    printf("-------------------------------------------------\n");
    printf("Measured time: %d us\n", delta);
    printf("-------------------------------------------------\n");

    average += ((delta - average)/(n + 1));
    n++;

    return delta;
}

uint32_t esp_code_timer_get_average(void)
{
    return (uint32_t)average;
}

void esp_code_timer_reset_average(void)
{
    average = 0;
    n = 0;
    ESP_LOGW(TAG, "Average calculation resat");
}

void _esp_code_timer_printer_task(void *arg)
{
    print_queue = xQueueCreate(PRINT_QUEUE_LENGTH, sizeof(esp_code_timer_t*));
    if (print_queue == NULL) {
        return;
    }

    ESP_LOGI(TAG, "Printer task started");

    while(1) {
        esp_code_timer_t* ct;
        xQueueReceive(print_queue, &ct, portMAX_DELAY);

        uint32_t last_timestamp = 0;
        printf("-------------------------------------------------\n");
        printf("%s\n",ct->timer_tag);
        printf("  #  tag   timestamp [us]       delta [us]\n");
        printf("-------------------------------------------------\n");

        if (ct->idx < 2) {
            ESP_LOGW(TAG, "There need to be at least two timestamps");
            printf("-------------------------------------------------\n");
            continue;
        }

        printf("%3d: %s \t%9d\t%s\n", 0, ct->buffer[0].tag, ct->buffer[0].timestamp, "     -");
        for(int i=1; i<ct->idx; i++) {
            printf("%3d: %s \t%9d\t%9d\n", i, ct->buffer[i].tag, ct->buffer[i].timestamp, ct->buffer[i].timestamp-last_timestamp);
            last_timestamp = ct->buffer[i].timestamp;
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        printf("-------------------------------------------------\n");

        if (ct->full) {
           ESP_LOGW(TAG, "Code timer buffer full, timestamps might have been lost");
        }
    }
}

esp_err_t esp_code_timer_init(esp_code_timer_t *ct, const char *timer_tag, size_t capacity)
{
    if (ct == NULL) {
        ESP_LOGE(TAG, "No object");
        return ESP_FAIL;
    }

    ct->capacity = capacity;

    if(capacity == 0) {
        ESP_LOGW(TAG,"Capacity = 0, setting to 10");
        ct->capacity = 10;
    }

    ct->buffer = (ct_timestamp_t*)malloc(sizeof(ct_timestamp_t)*ct->capacity);
    if (ct->buffer == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for code time buffer");
        return ESP_FAIL;
    }

    ct->idx = 0;
    ct->full = false;
    ct->active = true;
    ct->timer_tag = timer_tag;

    if (print_task == NULL) {
        xTaskCreate(_esp_code_timer_printer_task, "code_timer printer", 8192, ct, 2, &print_task);
        if (print_task == NULL) {
            ESP_LOGE(TAG, "Failed to create printer task");
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t esp_code_timer_deinit(esp_code_timer_t *ct)
{
    esp_err_t ret = ESP_FAIL;
    if (ct->buffer != NULL) {
        free(ct->buffer);
        ct->buffer = NULL;
        ret = ESP_OK;
    }
    return ret;
}

void esp_code_timer_activate_all(void)
{
    timers_active = true;
    ESP_LOGI(TAG, "Timers activated");
}

void esp_code_timer_deactivate_all(void)
{
    timers_active = false;
    ESP_LOGI(TAG, "Timers deactivated");
}

void esp_code_timer_activate(esp_code_timer_t *ct)
{
    ct->active = true;
}

void esp_code_timer_deactivate(esp_code_timer_t *ct)
{
    ct->active = false;
}

void esp_code_timer_take_timestamp(esp_code_timer_t *ct, char *tag)
{
    if((!timers_active) || (!ct->active) || (ct->full)) {
        return;
    }

    ct_timestamp_t timestamp = {
    .tag = tag,
    .timestamp = (uint32_t)esp_timer_get_time()
    };

    memcpy(&ct->buffer[ct->idx], &timestamp, sizeof(ct_timestamp_t));
    ct->idx++;

    if (ct->idx >= ct->capacity) {
        ct->full = true;
    }
}

void esp_code_timer_dump_timestamps(esp_code_timer_t *ct)
{
    xQueueSend(print_queue, &ct, pdMS_TO_TICKS(100));
}
