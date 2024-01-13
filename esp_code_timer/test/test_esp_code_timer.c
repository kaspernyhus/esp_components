/*
    Test of esp_signal_generator
*/

#include <limits.h>
#include <string.h>

#include "unity.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_testsuite.h"

TEST_CASE("Code timer simple", "[esp_code_timer]")
{
    uint32_t time_measured = 0;

    esp_code_timer_start();
    vTaskDelay(pdMS_TO_TICKS(100));
    time_measured = esp_code_timer_stop();

    TEST_ASSERT_LESS_OR_EQUAL_UINT32(110000, time_measured);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(90000, time_measured);

    ESP_LOGI("TAG", "time_measured: %d", time_measured);
}

TEST_CASE("Code timer simple average", "[esp_code_timer]")
{
    esp_code_timer_start();
    vTaskDelay(pdMS_TO_TICKS(320));
    esp_code_timer_stop();

    esp_code_timer_start();
    vTaskDelay(pdMS_TO_TICKS(123));
    esp_code_timer_stop();

    esp_code_timer_start();
    vTaskDelay(pdMS_TO_TICKS(429));
    esp_code_timer_stop();

    esp_code_timer_start();
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_code_timer_stop();

    uint32_t average = esp_code_timer_get_average();
    ESP_LOGI("TAG", "Average: %d", average);

    TEST_ASSERT_LESS_OR_EQUAL_UINT32(250000, average);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(200000, average);
}

TEST_CASE("Initialize/deinitialize object", "[esp_code_timer]")
{
    esp_code_timer_t ct;

    esp_code_timer_init(&ct, "Test timer", 5);
    TEST_ASSERT_NOT_NULL(ct.buffer);

    esp_code_timer_take_timestamp(&ct, "time 1");
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_code_timer_take_timestamp(&ct, "time 2");

    esp_code_timer_dump_timestamps(&ct);

    esp_code_timer_deinit(&ct);

    TEST_ASSERT_NULL(ct.buffer);
}
