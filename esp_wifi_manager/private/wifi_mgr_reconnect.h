/**
 * @file wifi_mgr_reconnect.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-07-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

/**
 * @brief Start reconnect loop. Will actively try to reconnect
 *        to a known AP.
 */
void wifi_mgr_start_reconnect_loop(void);
