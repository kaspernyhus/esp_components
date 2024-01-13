#include <stdio.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "wifi_manager.h"
#include "wifi_mgr_events.h"
#include "wifi_mgr_provisioning.h"

static const char* TAG = "WIFI-MGR-EXAMPLE";

static void wifi_mgr_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "WIFI_MGR_EVENT event_id: %s", wifi_mgr_event_name[event_id]);
}

void app_main(void)
{
    /* Initialize NVS flash for storing credentials */
    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        status = nvs_flash_init();
        ESP_ERROR_CHECK(status);
    }

    /* Create the default event loop used for sending event notifications */
    esp_event_loop_create_default();

    /* Start WiFi Manager task and wait a little to make sure it is running */
    xTaskCreate(wifi_mgr_task, "WiFi Manager Task", 4096, NULL, 2, NULL);
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Initialize network interface */
    esp_netif_init();

    /* Subscribe to events from WiFi Manager */
    esp_event_handler_instance_register(WIFI_MGR_EVENT, ESP_EVENT_ANY_ID, &wifi_mgr_event_handler, NULL, NULL);

    /* Start WiFi Manager */
    wifi_mgr_init_station();
    wifi_mgr_start();
}
