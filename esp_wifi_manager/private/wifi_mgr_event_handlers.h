/**
 * @file wifi_event_handlers.h
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
#include "wifi_mgr_events.h"

/**
 * @brief Register event handlers. Called by WiFi Manager when initialized.
 *
 */
esp_err_t wifi_mgr_register_event_handlers(void);

/**
 * @brief Post an event to WIFI_MGR_EVENT event loop.
 *        Other applications/modules/components can register to
 *        receive these events outside of the WiFi Manager.
 *
 * @param event to post
 */
void wifi_mgr_post_event(wifi_mgr_event_t event);
