/**
 * @file test_rtp.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <limits.h>
#include <string.h>

#include "unity.h"
#include "esp_log.h"

#include "rtp.h"

TEST_CASE("Initialization & deinit", "[rtp]")
{
    esp_err_t err;
    rtp_state_t state;

    err = rtp_session_init(0, 96);
    TEST_ESP_OK(err);

    err = rtp_session_init(RTP_SESSION_MAX+1, 96);
    TEST_ESP_ERR(err, ESP_FAIL);

    err = rtp_session_init(0, 96);
    TEST_ESP_ERR(err, ESP_FAIL);

    state = rtp_session_get_state(0);
    TEST_ASSERT_EQUAL_INT(RTP_STATE_INITIALIZED, state);

    err = rtp_session_deinit(0);
    TEST_ESP_OK(err);

    state = rtp_session_get_state(0);
    TEST_ASSERT_EQUAL_INT(RTP_STATE_INACTIVE, state);
}

TEST_CASE("Create RTP packet", "[rtp]")
{
    rtp_session_init(0, 96);

    uint8_t payload[4] = {0x12, 0x34, 0xab, 0xcd};
    uint8_t packet[100];

    size_t length = rtp_create_packet(0, payload, 4, packet, 100);

    TEST_ASSERT_EQUAL_INT(RTP_HEADER_SIZE+4, length);

    ESP_LOG_BUFFER_HEX("", packet, length);
}
