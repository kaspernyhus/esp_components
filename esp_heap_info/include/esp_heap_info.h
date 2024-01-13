/**
 * @file heap_info.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

/**
 * @brief Total system memory usage and low watermark
 *        (maximum systemwide peak memory usage up until now)
 *
 */
void esp_heap_info_dump(void);

/**
 * @brief Include SPIRAM (PSRAM) in info dump
 *
 */
void esp_heap_info_incl_spiram_dump(void);
