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

    while (1) {
        ESP_LOGI(TAG, "RED");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_RED, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "GREEN");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_GREEN, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "BLUE");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_BLUE, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "ORANGE");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_ORANGE, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "PURPLE");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_PURPLE, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "WHITE");
        esp_neopixel_set_color(&neopixel, NEOPIXEL_WHITE, 32u);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "OFF");
        esp_neopixel_off(&neopixel);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
