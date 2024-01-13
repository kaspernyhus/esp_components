/**
 * @file esp_neopixel.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "led_strip.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

#define MAX_BLINKING_NEOPIXELS 10

typedef enum {
    NEOPIXEL_WHITE,
    NEOPIXEL_ORANGE,
    NEOPIXEL_RED,
    NEOPIXEL_GREEN,
    NEOPIXEL_BLUE,
    NEOPIXEL_PURPLE,
} esp_neopixel_color_t;

typedef enum {
    NEOPIXEL_OFF,
    NEOPIXEL_ON,
} esp_neopixel_state_t;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} esp_neopixel_rgb_t;

typedef enum {
    NEOPIXEL_RGB,
    NEOPIXEL_GRB,
} esp_neopixel_format_t;

typedef struct {
    uint8_t rmt_channel;
    uint8_t gpio;
    bool invert;
    esp_neopixel_format_t format;
} esp_neopixel_config_t;

typedef struct {
    led_strip_handle_t led_strip;
    esp_neopixel_format_t format;
    esp_neopixel_rgb_t color;
    esp_neopixel_state_t state;
    TimerHandle_t blink_timer;
    SemaphoreHandle_t mutex;
} esp_neopixel_t;

/**
 * @brief Initialize neopixel
 *
 * @param neopixel
 * @param cfg
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_init(esp_neopixel_t* neopixel, esp_neopixel_config_t* cfg);

/**
 * @brief Turn neopixel on
 *
 * @param neopixel
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_on(esp_neopixel_t* neopixel);

/**
 * @brief Turn neopixel off
 *
 * @param neopixel
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_off(esp_neopixel_t* neopixel);

/**
 * @brief Set color
 *
 * @param neopixel
 * @param color
 * @param intensity
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_set_color(esp_neopixel_t* neopixel, esp_neopixel_color_t color, uint8_t intensity);

/**
 * @brief Set RGB color
 *
 * @param neopixel
 * @param rgb
 * @param intensity
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_set_pixel(esp_neopixel_t* neopixel, esp_neopixel_rgb_t rgb);

/**
 * @brief Start blinking the neopixel led at a fixed rate
 *
 * @param neopixel
 * @param period_ms
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_blink_start(esp_neopixel_t* neopixel, uint32_t period_ms);

/**
 * @brief Stop blinking the neopixel led
 *
 * @param neopixel
 * @return ESP_OK if successful, ESP_FAIL if not
 */
esp_err_t esp_neopixel_blink_stop(esp_neopixel_t* neopixel);
