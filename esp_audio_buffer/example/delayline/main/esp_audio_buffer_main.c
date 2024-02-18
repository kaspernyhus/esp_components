/**
 * @file esp_audio_buffer_main.c
 * @author Kasper Nyhus
 * @brief 
 * @version 0.1
 * @date 2024-02-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_audio_buffer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EXAMPLE_AUDIO_BUFFER_SIZE 1024
#define RW_BUFFER_SIZE 300

static const char* TAG = "EXAMPLE-AUDIO-BUFFER";
static esp_audio_buffer_t buffer;

void write_task(void *pvParameter)
{
    uint8_t data[RW_BUFFER_SIZE];
    for (int i = 0; i < RW_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    while (1)
    {
        ESP_LOGI(TAG, "Buffer write available: %ld", esp_audio_buffer_wr_avail(&buffer));
        esp_audio_buffer_write(&buffer, data, RW_BUFFER_SIZE);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void read_task(void *pvParameter)
{
    uint8_t read_data[RW_BUFFER_SIZE];
    while (1)
    {
        ESP_LOGI(TAG, "Buffer read available: %ld", esp_audio_buffer_rd_avail(&buffer));
        esp_audio_buffer_read(&buffer, read_data, RW_BUFFER_SIZE);
        ESP_LOG_BUFFER_HEX(TAG, read_data, RW_BUFFER_SIZE);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void app_main(void)
{
    esp_err_t err = esp_audio_buffer_create(&buffer, EXAMPLE_AUDIO_BUFFER_SIZE);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create buffer");
        return;
    }
    
    xTaskCreatePinnedToCore(write_task, "write_task", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(read_task, "read_task", 4096, NULL, 5, NULL, 1);

    vTaskDelay(portMAX_DELAY);
}
