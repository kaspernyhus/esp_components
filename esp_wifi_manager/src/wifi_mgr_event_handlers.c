/**
 * @file wifi_event_handlers.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "wifi_mgr_event_handlers.h"
#include "wifi_manager.h"
#include "wifi_mgr_events.h"
#include "wifi_mgr_prov.h"
#include "wifi_mgr_state.h"
#include "wifi_mgr_task.h"
#include "wifi_mgr_types.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

ESP_EVENT_DEFINE_BASE(WIFI_MGR_EVENT);

static esp_event_handler_instance_t instance_wifi_event;
static esp_event_handler_instance_t instance_ip_event;
static int retry_attempts = 0;

static const char* TAG = "WIFI-MGR-EVENT";

void wifi_mgr_post_event(wifi_mgr_event_t event)
{
    esp_event_post(WIFI_MGR_EVENT, event, NULL, 0, 0);
}

static wifi_mgr_fail_reason_t _handle_disconnect_event(void* event_data)
{
    wifi_mgr_fail_reason_t reason = WIFI_MGR_REASON_UNKNOWN;
    wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*)event_data;

    switch (disconnected->reason) {
    case WIFI_REASON_AUTH_EXPIRE:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_MIC_FAILURE:
        reason = WIFI_MGR_REASON_AUTH_ERROR;
        break;
    case WIFI_REASON_NO_AP_FOUND:
        reason = WIFI_MGR_REASON_AP_NOT_FOUND;
        break;
    default:
        break;
    }

    return reason;
}

/**
 * @brief Handle events from esp_wifi module
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
    case WIFI_EVENT_SCAN_DONE:
        ESP_LOGI(TAG, "[event] WiFi scan done");
        break;

    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "[event] WiFi station mode start");
        esp_wifi_connect();
        break;

    case WIFI_EVENT_STA_STOP:
        ESP_LOGI(TAG, "[event] WiFi station mode stop");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "[event] WiFi disconnected");
        wifi_mgr_post_event(WIFI_MGR_DISCONNECTED_EVENT);

        // TODO: needs clever logic here to determine
        //       - did provision fail?
        //       - did we lose an AP we were connected to?
        if (wifi_mgr_get_state() == WIFI_MGR_STATE_PROVISION_ACTIVE) {
            wifi_mgr_post_event(WIFI_MGR_PROVISION_FAILED_EVENT);
        }

        if (wifi_mgr_get_state() == WIFI_MGR_STATE_CONNECTED) {
            wifi_mgr_post_event(WIFI_MGR_AP_LOST_EVENT);
        }

        wifi_mgr_set_state(WIFI_MGR_STATE_NOT_CONNECTED);

        wifi_mgr_fail_reason_t reason = _handle_disconnect_event(event_data);
        ESP_LOGW(TAG, "Disconnect reason: %s", wifi_mgr_fail_reason[reason]);

        if (reason != WIFI_MGR_REASON_UNKNOWN) {
            // AP authentication error or AP not found
            //   - stop reconnecting and reset provisioning
            wifi_mgr_do_work(wifi_mgr_reset_provisioning);
            wifi_mgr_do_work(wifi_mgr_start_provisioning);
        } else {
            if (retry_attempts < CONFIG_WIFI_MGR_ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                retry_attempts++;
                ESP_LOGI(TAG, "#%d retry to connect to the AP", retry_attempts + 1);
            } else {
                retry_attempts = 0;
                wifi_mgr_do_work(wifi_mgr_reset_provisioning);
                wifi_mgr_do_work(wifi_mgr_start_provisioning);
            }
            ESP_LOGI(TAG, "connect to the AP fail");
        }
        break;

    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "[event] WiFi connected");
        if (wifi_mgr_get_state() == WIFI_MGR_STATE_PROVISION_ACTIVE) {
            wifi_mgr_post_event(WIFI_MGR_PROVISION_SUCCESS_EVENT);
        }
        break;

    case WIFI_EVENT_STA_BEACON_TIMEOUT:
        ESP_LOGI(TAG, "[event] WiFi beacon timeout.");
        break;

    default:
        ESP_LOGW(TAG, "Unhandled WIFI_EVENT event_id: %d", event_id);
        break;
    }
}

/**
 * @brief Handle events from esp_netif
 */
static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
        retry_attempts = 0;
        ESP_LOGI(TAG, "Got an IP");
        wifi_mgr_do_work(wifi_mgr_stop_provisioning);
        wifi_mgr_post_event(WIFI_MGR_CONNECTED_EVENT);
        wifi_mgr_set_state(WIFI_MGR_STATE_CONNECTED);
        break;
    case IP_EVENT_STA_LOST_IP:
        ESP_LOGI(TAG, "Lost IP");
        break;
    default:
        ESP_LOGW(TAG, "Unhandled IP_EVENT event_id: %d", event_id);
        break;
    }
}

/**
 * @brief Handle events from wifi_manager
 */
static void wifi_mgr_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
    case WIFI_MGR_PROVISION_ENDED_EVENT:
        ESP_LOGI(TAG, "[event] WIFI_MGR_PROVISION_ENDED_EVENT");
        wifi_mgr_do_work(wifi_mgr_deinit_provisioning);
        break;

    default:
        break;
    }
}

esp_err_t wifi_mgr_register_event_handlers(void)
{
    esp_err_t ret = ESP_OK;

    /* Subscribe to events from esp_wifi, esp_netif and WiFi Manager */
    ret |= esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_wifi_event);
    ret |= esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL, &instance_ip_event);
    ret |= esp_event_handler_instance_register(WIFI_MGR_EVENT, ESP_EVENT_ANY_ID, &wifi_mgr_event_handler, NULL, NULL);
    return ret;
}
