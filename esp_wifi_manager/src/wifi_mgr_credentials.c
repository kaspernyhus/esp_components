/**
 * @file wifi_mgr_credentials.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <string.h>

#include "wifi_mgr_credentials.h"
#include "wifi_mgr_provisioning.h"

#include "esp_log.h"
#include "esp_wifi.h"

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

static const char* TAG = "WIFI-MGR-UTILS";

bool wifi_is_provisioned(void)
{
    bool is_provisioned = false;
    wifi_config_t current_wifi_config;

    if (esp_wifi_get_config(WIFI_IF_STA, &current_wifi_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get WiFi configuration");
    }

    if (strlen((const char*)current_wifi_config.sta.ssid)) {
        is_provisioned = true;
    }
    return is_provisioned;
}

esp_err_t load_kconfig_credentials(void)
{
    esp_err_t ret = ESP_FAIL;
#ifdef CONFIG_WIFI_MGR_KCONFIG_CREDENTIALS
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        },
    };
    strncpy((char*)wifi_config.sta.ssid, CONFIG_WIFI_MGR_ESP_WIFI_SSID, MAX_SSID_LENGTH);
    strncpy((char*)wifi_config.sta.password, CONFIG_WIFI_MGR_ESP_WIFI_PASSWORD, MAX_PASSWORD_LENGTH);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Failed to set WiFi configuration. Error code: 0x%X (%s)", ret, esp_err_to_name(ret));
    }
#endif
    return ret;
}

void wifi_mgr_set_credentials(wifi_mgr_credentials_t* credentials)
{
    wifi_config_t wifi_cfg;

    esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);

    memcpy(wifi_cfg.sta.ssid, credentials->ssid, MAX_SSID_LENGTH);
    memcpy(wifi_cfg.sta.password, credentials->password, MAX_PASSWORD_LENGTH);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_connect();
}

void wifi_mgr_remove_credentials(void)
{
    esp_err_t ret;
    wifi_config_t current_wifi_config;

    ret = esp_wifi_get_config(WIFI_IF_STA, &current_wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get WiFi configuration: %s", esp_err_to_name(ret));
    }

    memset(current_wifi_config.sta.ssid, 0, MAX_SSID_LENGTH);
    memset(current_wifi_config.sta.password, 0, MAX_PASSWORD_LENGTH);

    ret = esp_wifi_set_config(WIFI_IF_STA, &current_wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration: %s", esp_err_to_name(ret));
    }
}
