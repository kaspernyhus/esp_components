/**
 * @file wifi_mgr_provisioning.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <stdint.h>

#define MAX_SSID_LENGTH 32
#define MAX_PASSWORD_LENGTH 64

/**
 * @brief WiFi Manager provision method
 */
typedef enum {
    WIFI_MGR_PROV_METHOD_KCONFIG,
    WIFI_MGR_PROV_METHOD_ESP,
    WIFI_MGR_PROV_METHOD_CUSTOM,
} wifi_mgr_prov_method_t;

typedef void (*wifi_prov_init_t)(void);
typedef void (*wifi_prov_deinit_t)(void);
typedef void (*wifi_prov_start_t)(void);
typedef void (*wifi_prov_stop_t)(void);
typedef void (*wifi_prov_reset_t)(void);

/**
 * @brief Defines the provisioning interface used by WiFi Manager
 *        to provide custom provisioning eg. via BLE.
 */
typedef struct {
    wifi_prov_init_t init;
    wifi_prov_deinit_t deinit;
    wifi_prov_start_t start;
    wifi_prov_stop_t stop;
    wifi_prov_reset_t reset;
    uint16_t post_stop_timeout;
} wifi_mgr_prov_t;

typedef struct {
    char ssid[MAX_SSID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} wifi_mgr_credentials_t;

/**
 * @brief Set the provisioning method
 *
 * @param wifi_mgr_prov_t method
 */
void wifi_mgr_set_provisioning_method(wifi_mgr_prov_t method);

/**
 * @brief Restart provisioning if timed out
 *
 */
void wifi_mgr_restart_provisioning(void);

/**
 * @brief Set SSID and password for the network
 *
 * @param credentials
 */
void wifi_mgr_set_credentials(wifi_mgr_credentials_t* credentials);
