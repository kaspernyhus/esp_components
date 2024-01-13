/**
 * @file heap_info.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "esp_heap_info.h"

#include "esp_heap_caps.h"
#include "esp_heap_task_info.h"
#include "esp_log.h"

typedef struct {
    size_t free_internal;
    size_t free_spiram;
    size_t free_total;
    int low_watermark_internal;
    int low_watermark_spiram;
    int low_watermark_total;
    size_t available_internal;
    size_t available_spiram;
    size_t available_total;
} system_info_heap_usage_t;

void system_info_get_system_heap_usage(system_info_heap_usage_t* heap_usage)
{
    const size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    const size_t free_spiram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    const size_t free_total = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    const int low_watermark_internal = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    const int low_watermark_spiram = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    const int low_watermark_total = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
    const size_t available_internal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    const size_t available_spiram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    const size_t available_total = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    heap_usage->free_internal = free_internal;
    heap_usage->free_spiram = free_spiram;
    heap_usage->free_total = free_total;
    heap_usage->low_watermark_internal = low_watermark_internal;
    heap_usage->low_watermark_spiram = low_watermark_spiram;
    heap_usage->low_watermark_total = low_watermark_total;
    heap_usage->available_internal = available_internal;
    heap_usage->available_spiram = available_spiram;
    heap_usage->available_total = available_total;
}

void esp_heap_info_dump(void)
{
    system_info_heap_usage_t heap_usage;
    system_info_get_system_heap_usage(&heap_usage);

    float heap_pct_free_internal = (float)heap_usage.free_internal / heap_usage.available_internal * 100;
    float heap_pct_lw_internal = (float)heap_usage.low_watermark_internal / heap_usage.available_internal * 100;

    printf("-------------------------------------------------\n");
    printf(" Heap usage\n");
    printf("-------------------------------------------------\n");
    printf(" Free Memory [bytes]\n");
    printf(" %7zu/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.free_internal, heap_usage.available_internal, 100 - heap_pct_free_internal, heap_pct_free_internal);
    printf("-------------------------------------------------\n");
    printf(" Low Watermarks - at lowest point recorded\n");
    printf(" %7d/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.low_watermark_internal, heap_usage.available_internal, 100 - heap_pct_lw_internal, heap_pct_lw_internal);
    printf("-------------------------------------------------\n");
}

void esp_heap_info_incl_spiram_dump(void)
{
    system_info_heap_usage_t heap_usage;
    system_info_get_system_heap_usage(&heap_usage);

    float heap_pct_free_internal = (float)heap_usage.free_internal / heap_usage.available_internal * 100;
    float heap_pct_free_spiram = (float)heap_usage.free_spiram / heap_usage.available_spiram * 100;
    float heap_pct_free_total = (float)heap_usage.free_total / heap_usage.available_total * 100;
    float heap_pct_lw_internal = (float)heap_usage.low_watermark_internal / heap_usage.available_internal * 100;
    float heap_pct_lw_spiram = (float)heap_usage.low_watermark_spiram / heap_usage.available_spiram * 100;
    float heap_pct_lw_total = (float)heap_usage.low_watermark_total / heap_usage.available_total * 100;

    printf("-------------------------------------------------\n");
    printf(" Heap usage\n");
    printf("-------------------------------------------------\n");
    printf(" Free Memory [bytes]\n");
    printf("  Internal: %7zu/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.free_internal, heap_usage.available_internal, 100 - heap_pct_free_internal, heap_pct_free_internal);
    printf("  PSRAM:    %7zu/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.free_spiram, heap_usage.available_spiram, 100 - heap_pct_free_spiram, heap_pct_free_spiram);
    printf("  Total:    %7zu/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.free_total, heap_usage.available_total, 100 - heap_pct_free_total, heap_pct_free_total);
    printf("-------------------------------------------------\n");
    printf(" Low Watermarks - at lowest point recorded\n");
    printf("  Internal: %7d/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.low_watermark_internal, heap_usage.available_internal, 100 - heap_pct_lw_internal, heap_pct_lw_internal);
    printf("  PSRAM:    %7d/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.low_watermark_spiram, heap_usage.available_spiram, 100 - heap_pct_lw_spiram, heap_pct_lw_spiram);
    printf("  Total:    %7d/%-7zu   %.2f%% used / %.2f%% free\n", heap_usage.low_watermark_total, heap_usage.available_total, 100 - heap_pct_lw_total, heap_pct_lw_total);
    printf("-------------------------------------------------\n");
}
