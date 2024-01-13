/**
 * @file wifi_mgr_credentials.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Check if there are WiFi credentials stored in flash
 *
 * @return true if found
 * @return false if not found
 */
bool wifi_is_provisioned(void);

/**
 * @brief Get the kconfig credentials and write them to wifi config in nvs
 *
 * @return esp_err_t ESP_OK on success
 */
esp_err_t load_kconfig_credentials(void);
