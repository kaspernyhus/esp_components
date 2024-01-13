/**
 * @file esp_signal_generator.h
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_sig_gen_config.h"
#include "freertos/FreeRTOS.h"

#define MAX_NUMBER_OF_CHANNELS 4

/* Forward declaration of signal generator struct */
typedef struct _esp_sig_gen esp_sig_gen_t;

typedef size_t (*process_func_t)(esp_sig_gen_t* sig_gen_handle, void* buffer, size_t number_of_bytes);

struct _esp_sig_gen {
    /* Common */
    int sample_rate;
    uint8_t channels;
    double volume;
    esp_sig_gen_audio_wave_t wave;
    esp_sig_gen_audio_format_t format;
    esp_sig_gen_padding_t padding;
    esp_sig_gen_sign_ext_t sign_extension;
    esp_sig_gen_audio_layout_t layout;
    esp_sig_gen_method_t method;

    /* Push method */
    size_t push_interval_ms;
    bool running;
    size_t samples_per_callback;

    /* Sine wave specific */
    double freq[MAX_NUMBER_OF_CHANNELS];

    /* Sine tab wave specific */
    size_t table_size;

    /* Private */
    double two_pi_over_sr;
    double phase_angle[MAX_NUMBER_OF_CHANNELS];
    double* wave_table;
    double scl;
    double scale;
    uint8_t bytes_per_sample;
    bool flip_endianess;
    process_func_t process;
};

/*
Configuration struct passed to init function
 - sample_rate is required
 - all other parameters have default values
*/
typedef struct {
    int sample_rate; // Required
    double freq[MAX_NUMBER_OF_CHANNELS];
    double volume;
    uint8_t channels;
    esp_sig_gen_audio_wave_t wave;
    esp_sig_gen_audio_format_t format;
    esp_sig_gen_padding_t padding;
    esp_sig_gen_sign_ext_t sign_extension;
    esp_sig_gen_audio_layout_t layout;
    esp_sig_gen_method_t method;
    size_t push_interval_ms;
}  esp_sig_gen_config_t;

/**
 * @brief Initialize signal generator with configuration struct. If cfg is empty default values are  used.
 *
 * @param sig_gen handle to instance
 * @param cfg configuration. Pass empty struct to initialize to default values set in menuconfig.
 * @return ESP_OK on successful initialization, ESP_FAIL otherwise
 */
esp_err_t esp_sig_gen_init(esp_sig_gen_t* sig_gen, esp_sig_gen_config_t* cfg);

/**
 * @brief Release resources used by signal generator
 *
 * @param sig_gen handle to instance
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t esp_sig_gen_deinit(esp_sig_gen_t* sig_gen);

/**
 * @brief Fill a user provided buffer with a total of samples number of samples.
 *
 * @param sig_gen handle to signal generator instance
 * @param buffer user provided buffer
 * @param size size of buffer in bytes
 * @param samples total number of samples to generate (across all channels)
 * @return total number of bytes generated
 */
size_t esp_sig_gen_fill(esp_sig_gen_t* sig_gen, void* buffer, size_t size, size_t samples);

/**
 * @brief Read samples from signal generator in intervals set by push_interval_ms in esp_sig_gen_init().
 *        Function blocks for max ticks_to_wait.
 * @note  Can only be used with ESP_SIG_GEN_SINE_TAB_WAVE!
 *
 * @param sig_gen handle to signal generator instance
 * @param dest user provided destination buffer to rad into
 * @param size maximum number of bytes to read out
 * @param bytes_read [out] number of bytes actually read out
 * @param ticks_to_wait
 * @return ESP_OK on success, ESP_FAIL if ticks_to_wait exceeded
 */
esp_err_t esp_sig_gen_read(esp_sig_gen_t* sig_gen, void* dest, size_t size, size_t* bytes_read, TickType_t ticks_to_wait);
