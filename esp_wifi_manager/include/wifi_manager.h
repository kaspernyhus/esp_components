/**
 * @file wifi_manager.h
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

/**
 * @brief FreeRTOS task must be created with xTaskCreate() before WiFi Manager is initialized
 *        Example: xTaskCreate(wifi_mgr_task, "WiFi Manager Task", 4096, NULL, 2, NULL);
 * @param param ** not used **
 */
void wifi_mgr_task(void* param);

/**
 * @brief Initialize WiFi to be in STATION mode and register event handlers
 *
 */
esp_err_t wifi_mgr_init_station(void);

/**
 * @brief Start WiFi Manager:
 *          1. Check for stored credentials
 *          2. Try to connect
 *          3. Start provisioning if configured
 */
esp_err_t wifi_mgr_start(void);

/**
 * @brief Attempt to reconnect if connection was lost.
 *
 */
void wifi_mgr_attempt_to_reconnect(void);

/**
 * @brief This call blocks until a WiFi connection has been established or timeout
 *
 * @param timeout_minutes timeout after x minutes. 0 = no timeout (will wait forever)
 * @return esp_err_t ESP_OK on connection, ESP_FAIL on timeout
 */
esp_err_t wifi_mgr_wait_for_connection(int timeout_minutes);

/**
 * @brief Remove the credentials stored in flash
 *
 */
void wifi_mgr_remove_credentials(void);

/**
 * @brief Reset all WiFi parameters
 *
 */
void wifi_mgr_reset(void);
