/**
 * @file wifi_mgr_types.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

/**
 * @brief WiFi STA connection fail reason
 */
#define FOREACH_REASON(REASON)         \
    REASON(WIFI_MGR_REASON_UNKNOWN)    \
    REASON(WIFI_MGR_REASON_AUTH_ERROR) \
    REASON(WIFI_MGR_REASON_AP_NOT_FOUND)

/**
 * @brief WiFi Manager internal state
 */
#define FOREACH_STATE(STATE)               \
    STATE(WIFI_MGR_STATE_ERROR)            \
    STATE(WIFI_MGR_STATE_INIT)             \
    STATE(WIFI_MGR_STATE_PROVISION_ACTIVE) \
    STATE(WIFI_MGR_STATE_CONNECTED)        \
    STATE(WIFI_MGR_STATE_NOT_CONNECTED)

/**
 * @brief WiFi STA connection fail reason
 */
typedef enum {
    FOREACH_REASON(GENERATE_ENUM)
        WIFI_MGR_REASON_MAX
} wifi_mgr_fail_reason_t;

static const char* const wifi_mgr_fail_reason[WIFI_MGR_REASON_MAX] = {
    FOREACH_REASON(GENERATE_STRING)
};

/**
 * @brief WiFi Manager internal state
 */
typedef enum {
    FOREACH_STATE(GENERATE_ENUM)
        WIFI_MGR_STATE_MAX
} wifi_mgr_state_t;

static const char* const wifi_mgr_state[WIFI_MGR_STATE_MAX] = {
    FOREACH_STATE(GENERATE_STRING)
};
