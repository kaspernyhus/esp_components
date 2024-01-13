/**
 * @file sine_tab.h
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "esp_signal_generator.h"
#include <stdbool.h>
#include "esp_err.h"

#define DEFAULT_TAB_SIZE 1024
#define DONT_USE_VOLUME false
#define USE_VOLUME true

/**
 * @brief initialize sine wave lookup table generation. Dynamically allocates table.
 *
 * @param sig_gen handle to signal generator instance
 * @param tab_size table size
 * @param use_volume include volume in table
 * @return ESP_OK on sucess, ESP_FAIL otherwise
 */
esp_err_t esp_sig_gen_sine_tab_init(esp_sig_gen_t* sig_gen, size_t tab_size, bool use_volume);

/**
 * @brief Release sine wave lookup table
 *
 * @param sig_gen handle to signal generator instance
 * @return ESP_OK on sucess, ESP_FAIL otherwise
 */
esp_err_t esp_sig_gen_sine_tab_deinit(esp_sig_gen_t* sig_gen);

/* Process functions */
size_t esp_sig_gen_create_sine_tab_16(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples);
size_t esp_sig_gen_create_sine_tab_24_32(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples);
