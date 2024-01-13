/**
 * @file wifi_mgr_esp_prov.h
 * @author Kasper Nyhus
 * @brief Interface for using Espressif provisioning with "ESP BLE Prov" App from a phone
 * @version 0.1
 * @date 2023-07-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

/**
 * @brief Initialize provisioning using Espressif provisioning manager component
 *
 */
void esp_prov_init(void);

/**
 * @brief Deinitialize provisioning
 *
 */
void esp_prov_deinit(void);

/**
 * @brief Start ESP_PROV App provisioning
 *
 */
void esp_prov_start(void);

/**
 * @brief Stop ESP_PROV App provisioning
 *
 */
void esp_prov_stop(void);

/**
 * @brief Reset provisioning
 *
 */
void esp_prov_reset(void);
