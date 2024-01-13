/**
 * @file code_timer.h
 * @author Kasper Nyhus
 * @brief Measure code execution time
 * @version 0.1
 * @date 2023-06-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

typedef struct {
  char *tag;
  uint32_t timestamp;
} ct_timestamp_t;

typedef struct {
  size_t capacity;
  ct_timestamp_t *buffer;
  size_t idx;
  const char *timer_tag;
  bool full;
  bool active;
} esp_code_timer_t;

/**
 * @brief Initialize code timer instance
 *
 * @param ct code timer instance
 * @param timer_tag string
 * @param capacity number of measurements
 */
esp_err_t esp_code_timer_init(esp_code_timer_t *ct, const char *timer_tag, size_t capacity);

/**
 * @brief Deinitialize code timer instance
 *
 * @param ct code timer instance
 * @return esp_err_t
 */
esp_err_t esp_code_timer_deinit(esp_code_timer_t *ct);

/**
 * @brief Activate all timers
 *
 */
void esp_code_timer_activate_all(void);

/**
 * @brief Deactivate all timers
 *
 */
void esp_code_timer_deactivate_all(void);

/**
 * @brief Activate specific timer instance
 *
 * @param ct code timer instance
 */
void esp_code_timer_activate(esp_code_timer_t *ct);

/**
 * @brief Deactivate specific timer instance
 *
 * @param ct code timer instance
 */
void esp_code_timer_deactivate(esp_code_timer_t *ct);

/**
 * @brief Record a timestamp
 *
 * @param ct code timer instance
 * @param tag
 */
void esp_code_timer_take_timestamp(esp_code_timer_t *ct, char *tag);

/**
 * @brief Dump timestamp data to terminal
 *
 * @param ct code timer instance
 */
void esp_code_timer_dump_timestamps(esp_code_timer_t *ct);

/**
 * @brief Single shot start
 *
 */
void esp_code_timer_start(void);

/**
 * @brief Single shot stop. Outputs time since 'code_timer_sh_start' was called
 *
 * @return uint32_t time since 'code_timer_sh_start' was called [us]
 */
uint32_t esp_code_timer_stop(void);

/**
 * @brief Get the average of all measurements taken between esp_code_timer_start and esp_code_timer_stop since reset
 *
 * @return uint32_t average
 */
uint32_t esp_code_timer_get_average(void);

/**
 * @brief Reset average calculation
 *
 */
void esp_code_timer_reset_average(void);
