/**
 * @file wifi_mgr_task.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "esp_err.h"
#include <stdbool.h>

#define WIFI_MGR_WORK_QUEUE_LEN 5

typedef void (*work_fn)(void);

/**
 * @brief Submit a work function to be executed bu the WiFi Manager work task.
 *        This means that functions can be "executed" from event loops and timers
 *        without overflowing their (relatively) small stacks.
 *
 * @param work
 * @return esp_err_t
 */
void wifi_mgr_do_work(work_fn work);

/**
 * @brief Return true if WiFi Manager task is started
 *
 */
bool is_wifi_mgr_task_started(void);
