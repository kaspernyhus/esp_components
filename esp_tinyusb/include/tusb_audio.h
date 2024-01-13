/**
 * @file tusb_audio.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2022-10-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "sdkconfig.h"
#include "tusb.h"
#include <stdint.h>

#if (CONFIG_TINYUSB_AUDIO_ENABLED != 1)
#error "TinyUSB AUDIO driver must be enabled in menuconfig"
#endif

/**
 * @brief Initialize USB Audio
 *
 * @return esp_err_t
 */
esp_err_t tusb_audio_init(void);

/**
 * @brief De-initialize USB audio driver
 *
 * @return esp_err_t
 */
esp_err_t tusb_audio_deinit(void);

#ifdef __cplusplus
}
#endif
