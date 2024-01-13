/**
 * @file wifi_mgr_provisioning.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdbool.h>
#include <string.h>

#include "wifi_mgr_event_handlers.h"
#include "wifi_mgr_events.h"
#include "wifi_mgr_prov.h"
#include "wifi_mgr_state.h"
#include "wifi_mgr_types.h"

#include "wifi_mgr_provisioning.h"

#include "esp_log.h"
#include "esp_wifi.h"

#include "freertos/timers.h"

static const char* TAG = "WIFI-MGR-PROV";

static wifi_mgr_prov_t prov;
static bool prov_method_set = false;
static TimerHandle_t wifi_mgr_prov_timeout_timer = NULL;
static int prov_timeout_minutes = CONFIG_WIFI_MGR_ESP_MAXIMUM_RETRY;
static bool prov_initialized = false;
static bool prov_started = false;

bool is_provisioning_initialized(void)
{
    return prov_initialized;
}

void wifi_mgr_set_provisioning_method(wifi_mgr_prov_t method)
{
    if (prov_method_set) {
        ESP_LOGE(TAG, "Provisioning method already initialized.");
        return;
    }

    memcpy(&prov, &method, sizeof(wifi_mgr_prov_t));

    if (prov.post_stop_timeout > 5001) {
        prov.post_stop_timeout = 0;
        ESP_LOGW(TAG, "Setting Post Stop Timeout to 0");
    }

    if ((prov.init != NULL) && (prov.deinit != NULL) && (prov.start != NULL) && (prov.stop != NULL)) {
        ESP_LOGI(TAG, "Provisioning method set");
        prov_method_set = true;
    }
}

static void _stop_provisioning(void)
{
    if (prov.stop != NULL) {
        prov.stop();
        prov_started = false;
    } else {
        ESP_LOGE(TAG, "Failed to stop provisioning method");
    }
    wifi_mgr_post_event(WIFI_MGR_PROVISION_ENDED_EVENT);
}

static void wifi_mgr_prov_timeout_cb(TimerHandle_t pxTimer)
{
    // TODO:
    // Shared so not nessesarily a timeout. can be post stop
    if (wifi_mgr_get_state() == WIFI_MGR_STATE_PROVISION_ACTIVE) {
        wifi_mgr_post_event(WIFI_MGR_PROVISION_TIMEOUT_EVENT);
    }
    _stop_provisioning();
}

static void create_timeout_timer(void)
{
    wifi_mgr_prov_timeout_timer = xTimerCreate(
        "WiFi provisioning max time",
        pdMS_TO_TICKS(prov_timeout_minutes * 1000 * 60),
        pdFALSE,
        (void*)0,
        wifi_mgr_prov_timeout_cb);
    if (wifi_mgr_prov_timeout_timer == NULL) {
        ESP_LOGE(TAG, "Provision timeout timer creation failed");
    }
}

static void start_timer(void)
{
    if (wifi_mgr_prov_timeout_timer == NULL) {
        ESP_LOGE(TAG, "wifi_mgr_prov_timeout_timer NULL");
        return;
    }

    if (xTimerStart(wifi_mgr_prov_timeout_timer, 0) == pdPASS) {
        ESP_LOGI(TAG, "WiFi provisioning timeout timer started: %d minutes", prov_timeout_minutes);
    } else {
        ESP_LOGE(TAG, "Failed to start WiFi provisioning timeout timer");
    }
}

static void stop_timer(void)
{
    if (wifi_mgr_prov_timeout_timer != NULL) {
        if (xTimerIsTimerActive(wifi_mgr_prov_timeout_timer)) {
            ESP_LOGI(TAG, "Stopping WiFi provisioning timeout timer.");
            xTimerStop(wifi_mgr_prov_timeout_timer, 0);
        }
    }
}

void wifi_mgr_init_provisioning(void)
{
    if (!prov_method_set) {
        ESP_LOGE(TAG, "Provisioning method not set.");
        return;
    }

    if (prov.init != NULL) {
        prov.init();
        prov_initialized = true;
        create_timeout_timer();
        wifi_mgr_post_event(WIFI_MGR_PROVISION_INIT_EVENT);
    } else {
        ESP_LOGE(TAG, "Failed to initialize provisioning method");
    }
}

void wifi_mgr_deinit_provisioning(void)
{
    if (prov.deinit != NULL) {
        prov.deinit();
        prov_initialized = false;
        wifi_mgr_post_event(WIFI_MGR_PROVISION_DEINIT_EVENT);
    } else {
        ESP_LOGE(TAG, "Failed to deinitialize provisioning method");
    }

    if (wifi_mgr_prov_timeout_timer != NULL) {
        xTimerDelete(wifi_mgr_prov_timeout_timer, 0);
    }
}

void wifi_mgr_start_provisioning(void)
{
    if (!prov_method_set) {
        ESP_LOGE(TAG, "Provisioning method not set.");
        return;
    }

    if (prov.start) {
        prov.start();
        prov_started = true;
        start_timer();
        wifi_mgr_set_state(WIFI_MGR_STATE_PROVISION_ACTIVE);
        wifi_mgr_post_event(WIFI_MGR_PROVISION_ACTIVE_EVENT);
    } else {
        ESP_LOGE(TAG, "Failed to start provisioning method");
    }
}

void wifi_mgr_stop_provisioning(void)
{
    if (!prov_method_set) {
        ESP_LOGE(TAG, "Provisioning method not set.");
        return;
    }

    if (!prov_started) {
        return;
    }

    stop_timer();

    /* Start a timer if pre stop time needed, else just call stop */
    if (prov.post_stop_timeout > 0) {
        xTimerChangePeriod(wifi_mgr_prov_timeout_timer, pdMS_TO_TICKS(prov.post_stop_timeout), 100);
        if (xTimerStart(wifi_mgr_prov_timeout_timer, 100) == pdPASS) {
            ESP_LOGI(TAG, "Pre stop timeout timer started: %d ms", prov.post_stop_timeout);
        } else {
            ESP_LOGE(TAG, "Failed to start Pre stop timeout timer");
        }
    } else {
        _stop_provisioning();
    }
}

void wifi_mgr_reset_provisioning(void)
{
    if (prov.reset) {
        prov.reset();
        prov_started = false;
    }
}

void wifi_mgr_restart_provisioning(void)
{
    wifi_mgr_start_provisioning();
}
