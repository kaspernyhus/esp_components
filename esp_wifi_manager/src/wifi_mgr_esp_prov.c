/**
 * @file wifi_mgr_esp_prov.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "wifi_mgr_esp_prov.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"

#include "esp_log.h"

#define SERVICE_NAME_PREFIX CONFIG_WIFI_MGR_ESP_PROV_SERVICE_NAME
#define CONFIG_ESP_PROV_POP "abcd1234"
#define BOARD_SERIAL_NUMBER_STR_SIZE 16

static const char* TAG = "ESP_APP_PROV";

void esp_prov_init(void)
{
    esp_err_t ret;
    wifi_prov_mgr_config_t prov_mgr_config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };
    ret = wifi_prov_mgr_init(prov_mgr_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize provisioning manager: %s", esp_err_to_name(ret));
    }
}

void esp_prov_start(void)
{
    ESP_LOGI(TAG, "Starting provisioning");
    char service_name[MAX_BLE_DEVNAME_LEN];
    uint8_t serial_number[] = "123456789";
    snprintf(service_name, MAX_BLE_DEVNAME_LEN, "%s%.10s", SERVICE_NAME_PREFIX, serial_number);

    /* What is the security level that we want (0, 1, 2):
     *  - WIFI_PROV_SECURITY_0 is simply plain text communication.
     *  -  WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
     *     using X25519 key exchange and proof of possession (pop) and AES-CTR
     *     for encryption/decryption of messages.
     *  - WIFI_PROV_SECURITY_2 SRP6a based authentication and key exchange
     *   + AES-GCM encryption/decryption of messages
     */
#ifdef CONFIG_WIFI_PROV_SEC_0
    wifi_prov_security_t security = WIFI_PROV_SECURITY_0;
#else
    wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
#endif
    const char* pop = CONFIG_ESP_PROV_POP;

    /* This will set a custom 128 bit UUID which will be included in the BLE advertisement
     * and will correspond to the primary GATT service that provides provisioning
     * endpoints as GATT characteristics. Each GATT characteristic will be
     * formed using the primary service UUID as base, with different auto assigned
     * 12th and 13th bytes (assume counting starts from 0th byte). The client side
     * applications must identify the endpoints by reading the User Characteristic
     * Description descriptor (0x2901) for each characteristic, which contains the
     * endpoint name of the characteristic */
    uint8_t custom_service_uuid[] = {
        /* LSB <------------------------------------------------------------------------------> MSB */
        0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02
    };
    wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
    const char* service_key = NULL;
    wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key);
}

void esp_prov_stop(void)
{
    wifi_prov_mgr_stop_provisioning();
}

void esp_prov_reset(void)
{
    ESP_LOGW(TAG, "wifi_prov_mgr_reset_sm_state_on_failure");
    esp_err_t ret;
    ret = wifi_prov_mgr_reset_sm_state_on_failure();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset prov mgr: %s", esp_err_to_name(ret));
    }
}

void esp_prov_deinit(void)
{
    ESP_LOGI(TAG, "wifi_prov_mgr_deinit");
    wifi_prov_mgr_deinit();
}
