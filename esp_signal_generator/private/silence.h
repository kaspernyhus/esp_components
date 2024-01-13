/**
 * @file silence.h
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

size_t esp_sig_gen_create_silence(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples);
