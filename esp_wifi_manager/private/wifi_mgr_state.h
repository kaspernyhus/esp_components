/**
 * @file wifi_mgr_state.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "wifi_mgr_types.h"

/**
 * @brief Set the WiFi Manager internal state
 */
void wifi_mgr_set_state(wifi_mgr_state_t new_state);

/**
 * @brief Get the WiFi Manager internal state
 */
wifi_mgr_state_t wifi_mgr_get_state(void);
