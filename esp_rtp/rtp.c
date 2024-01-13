/**
 * @file rtp.c
 * @author Kasper Nyhus Kaae (KANK)
 * @brief
 * @version 0.1
 * @date 2022-10-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "rtp.h"

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "lwip/sockets.h"

static const char* TAG = "RTP";

static rtp_session_t active_sessions[RTP_SESSION_MAX];

esp_err_t rtp_session_init(uint8_t session, uint8_t payload_type)
{
    if (session > RTP_SESSION_MAX) {
        ESP_LOGE(TAG, "Session above RTP_SESSION_MAX");
        return ESP_FAIL;
    }

    if (active_sessions[session].state != RTP_STATE_INACTIVE) {
        ESP_LOGE(TAG, "Session already initialized");
        return ESP_FAIL;
    }

    // Initialize RTP header
    active_sessions[session].header = (rtp_header_t) {
        .version = 2,
        .padding = 0,
        .extension = 0,
        .csrc_count = 0,
        .marker = 1,
        .payload_type = payload_type,
        .sequence_number = esp_random(),
        .timestamp = (uint32_t)esp_random(),
        .ssrc = esp_random()
    };

    active_sessions[session].state = RTP_STATE_INITIALIZED;
    active_sessions[session].esp_timestamp = esp_timer_get_time();
    ESP_LOGI(TAG, "RTP session %d initialized.", session);

    return ESP_OK;
}

esp_err_t rtp_session_deinit(uint8_t session)
{
    if (session > RTP_SESSION_MAX) {
        ESP_LOGE(TAG, "Session above RTP_SESSION_MAX");
        return ESP_FAIL;
    }

    active_sessions[session].state = RTP_STATE_INACTIVE;
    return ESP_OK;
}

void rtp_session_set_timestamp_inc(uint8_t session, size_t timestamp_inc)
{
    active_sessions[session].timestamp_inc = timestamp_inc;
}

rtp_state_t rtp_session_get_state(uint8_t session)
{
    if (session > RTP_SESSION_MAX) {
        ESP_LOGE(TAG, "Session above RTP_SESSION_MAX");
        return ESP_FAIL;
    }

    return active_sessions[session].state;
}

static size_t rtp_serialize(rtp_header_t header, uint8_t* payload, size_t payload_length, uint8_t* rtp_packet)
{
    size_t total_length = 0;
    if (payload_length == 0) {
        return total_length;
    }

    // Convert RTP header to Big Endian
    uint16_t* buffer = (uint16_t*)rtp_packet;
    buffer[0] = (header.version << 14) | (0 << 13) | (header.extension << 12) | (header.csrc_count << 8) | (header.marker << 7) | (header.payload_type);
    buffer[1] = header.sequence_number;
    buffer[2] = header.timestamp >> 16;
    buffer[3] = header.timestamp;
    buffer[4] = header.ssrc >> 16;
    buffer[5] = header.ssrc;

    for (int i = 0; i < (RTP_HEADER_SIZE >> 1); i++) {
        buffer[i] = htons(buffer[i]);
    }

    // TODO: remove above code and use this instead:
    // *(rtp_header_t*)rtp_packet = header;
    // (requires fiddling around with endian swapping afterwards)

    // Add payload
    uint8_t* destination = rtp_packet + RTP_HEADER_SIZE;
    memcpy(destination, payload, payload_length);

    total_length = RTP_HEADER_SIZE + payload_length;
    return total_length;
}

size_t rtp_create_packet(uint8_t session, void* payload, size_t payload_length, void* output, size_t size)
{
    if (session > RTP_SESSION_MAX) {
        ESP_LOGE(TAG, "Session above RTP_SESSION_MAX");
        return 0;
    }

    if (size < (RTP_HEADER_SIZE + payload_length)) {
        ESP_LOGE(TAG, "Output buffer too small to hold RTP packet");
        return 0;
    }

    if (active_sessions[session].state == RTP_STATE_INACTIVE) {
        ESP_LOGE(TAG, "RTP session %d inactive", session);
        return 0;
    }

    active_sessions[session].header.sequence_number++;
    active_sessions[session].header.timestamp += active_sessions[session].timestamp_inc;

    size_t output_size = rtp_serialize(active_sessions[session].header, payload, payload_length, output);
    if (active_sessions[session].header.marker == 1u) { // Change the market to 0 after building the first packet.
        active_sessions[session].header.marker = 0;
    }

    return output_size;
}
