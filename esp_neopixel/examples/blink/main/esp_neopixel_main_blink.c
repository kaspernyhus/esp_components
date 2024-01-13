/**
 * @file main.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-06-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_neopixel.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip_types.h"


static const char* TAG = "EXAMPLE-NEOPIXEL";


void app_main(void)
{
    esp_neopixel_t neopixel;
    esp_neopixel_config_t cfg = {
        .rmt_channel = 0,
        .gpio = 48,
        .invert = false,
        .format = NEOPIXEL_RGB,
    };
    ESP_ERROR_CHECK(esp_neopixel_init(&neopixel, &cfg));

    esp_neopixel_set_color(&neopixel, NEOPIXEL_ORANGE, 32u);

    ESP_LOGI(TAG, "Starting blink");
    esp_neopixel_blink_start(&neopixel, 10);

    vTaskDelay(pdMS_TO_TICKS(3000));
    esp_neopixel_set_color(&neopixel, NEOPIXEL_GREEN, 32u);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_neopixel_set_color(&neopixel, NEOPIXEL_BLUE, 32u);

    vTaskDelay(pdMS_TO_TICKS(300));
    esp_neopixel_set_color(&neopixel, NEOPIXEL_RED, 32u);

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Stopping blink");
    esp_neopixel_blink_stop(&neopixel);
}
