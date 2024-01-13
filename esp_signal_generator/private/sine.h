/**
 * @file sine.h
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

/**
 * @brief Initialize sine wave generation
 *
 * @param sig_gen handle to signal generator instance
 */
void esp_sig_gen_sine_init(esp_sig_gen_t* sig_gen);

/* Process functions */
size_t esp_sig_gen_create_sine_16(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples);
size_t esp_sig_gen_create_sine_24_32(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples);
