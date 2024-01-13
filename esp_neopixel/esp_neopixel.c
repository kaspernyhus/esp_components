/**
 * @file esp_neopixel.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "esp_neopixel.h"
#include "esp_log.h"
#include "include/esp_neopixel.h"
#include <stdint.h>


static const char* TAG = "ESP_NEOPIXEL";

static esp_neopixel_t* neopixels_blinking[MAX_BLINKING_NEOPIXELS];

static void blink_cb(TimerHandle_t xTimer)
{
    uint32_t blink_id = (uint32_t)pvTimerGetTimerID(xTimer);
    esp_neopixel_t* neopixel = neopixels_blinking[blink_id];

    if (neopixel == NULL) {
        return;
    }

    if (neopixel->state == NEOPIXEL_ON) {
        esp_neopixel_off(neopixel);
    } else {
        esp_neopixel_on(neopixel);
    }
}

static bool is_blinking(esp_neopixel_t* neopixel)
{
    if (neopixel->blink_timer != NULL) {
        return true;
    }
    return false;
}

esp_err_t esp_neopixel_init(esp_neopixel_t* neopixel, esp_neopixel_config_t* cfg)
{
    if (neopixel == NULL) {
        ESP_LOGE(TAG, "Neopixel object is NULL");
        return ESP_FAIL;
    }

    led_strip_config_t strip_config = {
        .strip_gpio_num = cfg->gpio,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = cfg->invert,
    };

    // LED strip backend configuration
    led_strip_rmt_config_t rmt_config = {
        /* Old RMT driver config */
        .rmt_channel = cfg->rmt_channel,

        /* New RMT driver config */
        // .clk_src = RMT_CLK_SRC_DEFAULT,
        // .resolution_hz = (10 * 1000 * 1000),
        // .flags.with_dma = false,
    };


    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &neopixel->led_strip);

    if ((err != ESP_OK) || (neopixel->led_strip == NULL)) {
        ESP_LOGE(TAG, "Failed to create LED strip object");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");

    neopixel->format = cfg->format;
    neopixel->color = (esp_neopixel_rgb_t) { 0 };
    neopixel->blink_timer = NULL;
    neopixel->mutex = xSemaphoreCreateMutex();

    return ESP_OK;
}

esp_err_t esp_neopixel_set_pixel(esp_neopixel_t* neopixel, esp_neopixel_rgb_t rgb)
{
    esp_err_t ret = ESP_FAIL;

    if (neopixel->led_strip == NULL) {
        ESP_LOGE(TAG, "LED strip object is NULL");
        return ESP_FAIL;
    }

    if (xSemaphoreTake(neopixel->mutex, 0)) {

        switch (neopixel->format) {
        case NEOPIXEL_GRB:
            ret = led_strip_set_pixel(neopixel->led_strip, 0, rgb.green, rgb.red, rgb.blue);
            break;
        case NEOPIXEL_RGB:
            ret = led_strip_set_pixel(neopixel->led_strip, 0, rgb.red, rgb.green, rgb.blue);
            break;
        default:
            ESP_LOGE(TAG, "Unknown pixel format");
            break;
        }
        if (ret == ESP_OK) {
            neopixel->color = rgb;
            ret = led_strip_refresh(neopixel->led_strip);
            if (ret == ESP_OK) {
                ESP_LOGD(TAG, "Set pixel to: %d, %d, %d", rgb.red, rgb.green, rgb.blue);
                neopixel->state = NEOPIXEL_ON;
            }
        }

        if (is_blinking(neopixel)) {
            xTimerReset(neopixel->blink_timer, 0);
        }

        xSemaphoreGive(neopixel->mutex);
    }
    return ret;
}

esp_err_t esp_neopixel_on(esp_neopixel_t* neopixel)
{
    esp_err_t ret = ESP_FAIL;
    if (neopixel->led_strip != NULL) {
        ret = esp_neopixel_set_pixel(neopixel, neopixel->color);
    }
    return ret;
}

esp_err_t esp_neopixel_off(esp_neopixel_t* neopixel)
{
    esp_err_t ret = ESP_FAIL;
    if (neopixel->led_strip != NULL) {
        if (xSemaphoreTake(neopixel->mutex, 0)) {
            ret = led_strip_clear(neopixel->led_strip);
            neopixel->state = NEOPIXEL_OFF;
            xSemaphoreGive(neopixel->mutex);
        }
    }
    return ret;
}

esp_err_t esp_neopixel_set_color(esp_neopixel_t* neopixel, esp_neopixel_color_t color, uint8_t intensity)
{
    esp_err_t ret = ESP_FAIL;
    if (neopixel->led_strip == NULL) {
        ESP_LOGE(TAG, "LED strip object is NULL");
        return ESP_FAIL;
    }

    esp_neopixel_rgb_t rgb = { 0 };
    switch (color) {
    case NEOPIXEL_RED:
        rgb.red = intensity;
        break;
    case NEOPIXEL_GREEN:
        rgb.green = intensity;
        break;
    case NEOPIXEL_BLUE:
        rgb.blue = intensity;
        break;
    case NEOPIXEL_WHITE:
        rgb.red = intensity;
        rgb.green = intensity;
        rgb.blue = intensity;
        break;
    case NEOPIXEL_ORANGE:
        rgb.red = intensity;
        rgb.green = intensity / 3;
        break;
    case NEOPIXEL_PURPLE:
        rgb.red = intensity / 3;
        rgb.blue = intensity;
        break;
    default:
        break;
    }
    ret = esp_neopixel_set_pixel(neopixel, rgb);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set pixel");
    }
    return ret;
}

esp_err_t esp_neopixel_blink_start(esp_neopixel_t* neopixel, uint32_t period_ms)
{
    esp_err_t ret = ESP_FAIL;
    if (neopixel->led_strip == NULL) {
        ESP_LOGE(TAG, "LED strip object is NULL");
        return ESP_FAIL;
    }

    uint32_t blink_id;
    for (blink_id = 0; blink_id < MAX_BLINKING_NEOPIXELS; blink_id++) {
        if (neopixels_blinking[blink_id] == NULL) {
            break;
        }
    }
    if (blink_id == MAX_BLINKING_NEOPIXELS) {
        ESP_LOGE(TAG, "Too many blinking neopixels");
        return ESP_FAIL;
    }

    neopixel->blink_timer = xTimerCreate("blink_timer", period_ms / portTICK_PERIOD_MS, pdTRUE, (void*)blink_id, blink_cb);
    if (neopixel->blink_timer != NULL) {
        neopixels_blinking[blink_id] = neopixel;
        xTimerStart(neopixel->blink_timer, 0);
        ret = ESP_OK;
    }

    return ret;
}

esp_err_t esp_neopixel_blink_stop(esp_neopixel_t* neopixel)
{
    esp_err_t ret = ESP_FAIL;
    if (neopixel->led_strip == NULL) {
        ESP_LOGE(TAG, "LED strip object is NULL");
        return ESP_FAIL;
    }

    if (neopixel->blink_timer == NULL) {
        ESP_LOGE(TAG, "Blink timer is NULL");
        return ESP_FAIL;
    }

    xTimerStop(neopixel->blink_timer, 0);
    xTimerDelete(neopixel->blink_timer, 0);
    neopixel->blink_timer = NULL;

    for (uint32_t blink_id = 0; blink_id < MAX_BLINKING_NEOPIXELS; blink_id++) {
        if (neopixels_blinking[blink_id] == neopixel) {
            neopixels_blinking[blink_id] = NULL;
            break;
        }
    }
    esp_neopixel_off(neopixel);
    ret = ESP_OK;
    return ret;
}
