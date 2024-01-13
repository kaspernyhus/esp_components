/**
 * @file wifi_mgr_provisioniong.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <stdbool.h>

/**
 * @brief Initialize provisioning and create timers needed by
 *        WiFi Manager to manage provisioning
 */
void wifi_mgr_init_provisioning(void);

/**
 * @brief Release all resources
 */
void wifi_mgr_deinit_provisioning(void);

/**
 * @brief Start active provisioning process
 */
void wifi_mgr_start_provisioning(void);

/**
 * @brief Stop active provisioning process
 */
void wifi_mgr_stop_provisioning(void);

/**
 * @brief Reset provisioning parameters
 */
void wifi_mgr_reset_provisioning(void);

/**
 * @brief Return true if provisioning method has been initialized.
 */
bool is_provisioning_initialized(void);
