/**
 * @file wifi_manager.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <string.h>

#include "wifi_manager.h"
#include "wifi_mgr_credentials.h"
#include "wifi_mgr_esp_prov.h"
#include "wifi_mgr_event_handlers.h"
#include "wifi_mgr_events.h"
#include "wifi_mgr_prov.h"
#include "wifi_mgr_provisioning.h"
#include "wifi_mgr_reconnect.h"
#include "wifi_mgr_state.h"
#include "wifi_mgr_task.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#ifdef CONFIG_WIFI_MGR_ESP_PROV
static const wifi_mgr_prov_method_t prov_method = WIFI_MGR_PROV_METHOD_ESP;
#elif CONFIG_WIFI_MGR_CUSTOM_PROV
static const wifi_mgr_prov_method_t prov_method = WIFI_MGR_PROV_METHOD_CUSTOM;
#else
static const wifi_mgr_prov_method_t prov_method = WIFI_MGR_PROV_METHOD_KCONFIG;
#endif

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_mgr_event_group;
static TimerHandle_t wifi_mgr_reconnect_timer = NULL;
static wifi_mgr_state_t internal_state = WIFI_MGR_STATE_INIT;

static const char* TAG = "WIFI-MGR";

void wifi_mgr_set_state(wifi_mgr_state_t new_state)
{
    // TODO: Lock with a mutex
    internal_state = new_state;
    ESP_LOGI(TAG, "[state] %s", wifi_mgr_state[internal_state]);
}

wifi_mgr_state_t wifi_mgr_get_state(void)
{
    // TODO: Lock with a mutex
    return internal_state;
}

static void attempt_reconnect(void)
{
    wifi_config_t current_wifi_config;
    if (esp_wifi_get_config(WIFI_IF_STA, &current_wifi_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get WiFi configuration");
        return;
    }

    ESP_LOGI(TAG, "Attempting to reconnect to SSID: %s", current_wifi_config.sta.ssid);
    esp_wifi_connect();
}

static void reconnect_cb(TimerHandle_t pxTimer)
{
    attempt_reconnect();
}

static void create_reconnect_timer(void)
{
    wifi_mgr_reconnect_timer = xTimerCreate(
        "WiFi provisioning max time",
        pdMS_TO_TICKS(10000),
        pdTRUE,
        (void*)0,
        reconnect_cb);
    if (wifi_mgr_reconnect_timer == NULL) {
        ESP_LOGE(TAG, "Reconnect timer creation failed");
    }
}

static void stop_reconnect_timer(void)
{
    if (wifi_mgr_reconnect_timer != NULL) {
        if (xTimerIsTimerActive(wifi_mgr_reconnect_timer)) {
            ESP_LOGI(TAG, "Stopping WiFi reconnect timer.");
            xTimerStop(wifi_mgr_reconnect_timer, 0);
        }
    }
}

static void wifi_mgr_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_MGR_CONNECTED_EVENT) {
        xEventGroupSetBits(wifi_mgr_event_group, WIFI_CONNECTED_BIT);
        stop_reconnect_timer();
    }
}

esp_err_t wifi_mgr_init_station(void)
{
    esp_err_t ret;
    bool error = false;

    if (!is_wifi_mgr_task_started()) {
        ESP_LOGE(TAG, "Call xTaskCreate with wifi_mgr_task before initializing WiFi Manager");
        return ESP_FAIL;
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(ret));
        error = true;
    }

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(ret));
        error = true;
    }

    ret = wifi_mgr_register_event_handlers();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "wifi_mgr_register_event_handlers failed");
        error = true;
    }

    ret = esp_event_handler_instance_register(WIFI_MGR_EVENT, WIFI_MGR_CONNECTED_EVENT, &wifi_mgr_event_handler, NULL, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed");
        error = true;
    }

    wifi_mgr_event_group = xEventGroupCreate();
    if (wifi_mgr_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        error = true;
    }

    if (error) {
        ret = ESP_FAIL;
        wifi_mgr_set_state(WIFI_MGR_STATE_ERROR);
    }

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi Manager initialized.");
        wifi_mgr_set_state(WIFI_MGR_STATE_INIT);
    }

    return ret;
}

esp_err_t wifi_mgr_start(void)
{
    esp_err_t ret;

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed with: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Starting WiFi in STA mode");

    create_reconnect_timer();

#ifdef CONFIG_WIFI_MGR_FORCE_CREDENTIALS
    ESP_LOGW(TAG, "Forcing hardcoded credentials");
    load_kconfig_credentials();
#endif

    /* Setup ESP App Provisioning */
    if (prov_method == WIFI_MGR_PROV_METHOD_ESP) {
        wifi_mgr_prov_t prov_config = {
            .init = esp_prov_init,
            .deinit = esp_prov_deinit,
            .start = esp_prov_start,
            .stop = esp_prov_stop,
            .reset = NULL,
            .post_stop_timeout = 5000,
        };
        wifi_mgr_set_provisioning_method(prov_config);
    }

    /* Check to see if there are credentials stored in nvs */
    if (!wifi_is_provisioned()) {
        ESP_LOGW(TAG, "No credentials in nvs");
        wifi_mgr_post_event(WIFI_MGR_NO_CONNECTION_EVENT);

        if ((prov_method == WIFI_MGR_PROV_METHOD_ESP) || (prov_method == WIFI_MGR_PROV_METHOD_CUSTOM)) {
            wifi_mgr_do_work(wifi_mgr_init_provisioning);
            wifi_mgr_do_work(wifi_mgr_start_provisioning);
        } else {
            ESP_LOGI(TAG, "Getting credentials from menuconfig (Kconfig)");
            ret = load_kconfig_credentials();
        }
    } else {
        wifi_config_t current_wifi_config;
        ret = esp_wifi_get_config(WIFI_IF_STA, &current_wifi_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get WiFi configuration");
            return ret;
        }
        ESP_LOGI(TAG, "Found credentials in nvs for SSID: %s", current_wifi_config.sta.ssid);
    }

    if (ret == ESP_OK) {
        esp_wifi_connect();
    }

    return ret;
}

esp_err_t wifi_mgr_wait_for_connection(int timeout_minutes)
{
    esp_err_t connection = ESP_FAIL;
    TickType_t ticks_to_wait;

    if (timeout_minutes > 0) {
        ticks_to_wait = pdMS_TO_TICKS(timeout_minutes * 1000 * 60);
    } else {
        ticks_to_wait = portMAX_DELAY;
    }

    /* Wait on event notification or timeout */
    EventBits_t event_bits = xEventGroupWaitBits(wifi_mgr_event_group, WIFI_CONNECTED_BIT, pdTRUE, pdFALSE, ticks_to_wait);
    if (event_bits & WIFI_CONNECTED_BIT) {
        connection = ESP_OK;
    } else {
        ESP_LOGE(TAG, "wifi_mgr_wait_for_connection timed out.");
    }
    return connection;
}

void wifi_mgr_reset(void)
{
    if (esp_wifi_restore() == ESP_OK) {
        ESP_LOGI(TAG, "All WiFi parameters reset to default values");
    }
}

void wifi_mgr_start_reconnect_loop(void)
{
    if (!wifi_is_provisioned()) {
        ESP_LOGW(TAG, "No valid credentials in nvs.");
        return;
    }

    if (xTimerStart(wifi_mgr_reconnect_timer, 0) == pdPASS) {
        ESP_LOGI(TAG, "WiFi reconnect timer started: %d sec", 10000);
    } else {
        ESP_LOGE(TAG, "Failed to start WiFi reconnect timer");
    }
}

void wifi_mgr_attempt_to_reconnect(void)
{
    if (!wifi_is_provisioned()) {
        ESP_LOGW(TAG, "No valid credentials in nvs.");
        return;
    }
    attempt_reconnect();
}
