/**
 * @file wifi_mgr_task.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "wifi_mgr_task.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char* TAG = "WIFI-MGR-TASK";

static QueueHandle_t work_queue = NULL;
static bool task_started = false;

void wifi_mgr_do_work(work_fn work)
{
    if (work_queue == NULL) {
        ESP_LOGE(TAG, "work_queue not initialized");
        return;
    }

    ESP_LOGD(TAG, "sending work");
    if (xQueueSend(work_queue, &work, 0) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to send work to task");
    }
}

/**
 * @brief This task sleeps waiting for work to be posted on the work_queue.
 *        Work is presented as a function pointer that the task pops off
 *        the queue and simply executes. This means that work can be requested
 *        from the context of event loops and from timers but run in the
 *        context of the WiFi Manager task with a large enough stack.
 *
 * @param param ** not used **
 */
void wifi_mgr_task(void* param)
{
    work_queue = xQueueCreate(WIFI_MGR_WORK_QUEUE_LEN, sizeof(work_fn));
    if (work_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create work queue. Terminating WiFi Manager work task");
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "WiFi Manager task started");
    task_started = true;

    while (1) {
        ESP_LOGD(TAG, "Waiting on work");
        work_fn work;
        xQueueReceive(work_queue, &work, portMAX_DELAY);
        ESP_LOGD(TAG, "Doing work");
        work();
    }
    task_started = false;
}

bool is_wifi_mgr_task_started(void)
{
    return task_started;
}
