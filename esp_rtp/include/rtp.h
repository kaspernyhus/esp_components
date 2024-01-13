/**
 * @file rtp.h
 * @author Kasper Nyhus Kaae (KANK)
 * @author Lars Peter Jensen (LAPJ)
 * @author Eduard Steven Macías Uriña (ESMU)
 * @brief
 * @version 0.1
 * @date 2022-10-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <esp_system.h>

#define RTP_SESSION_MAX 4
#define RTP_HEADER_SIZE 12

typedef enum {
    RTP_STATE_INACTIVE,
    RTP_STATE_INITIALIZED,
    RTP_STATE_MAX
} rtp_state_t;

typedef struct {
    uint8_t version : 2; // version
    uint8_t padding : 1; // padding
    uint8_t extension : 1; // extension bit
    uint8_t csrc_count : 4; // CSRC count
    uint8_t marker : 1; // marker bit
    uint8_t payload_type : 7; // payload type
    uint16_t sequence_number : 16; // sequence number
    uint32_t timestamp : 32; // timestamp
    uint32_t ssrc : 32; // synchronization source (SSRC) identifier
} __attribute__((packed)) rtp_header_t;

typedef struct {
    rtp_header_t header;
    int64_t esp_timestamp;
    size_t timestamp_inc;
    rtp_state_t state;
} rtp_session_t;

/**
 * @brief Initialization of the RTP session
 *
 * @param session identifier
 * @return esp_err_t ESP_OK on success
 */
esp_err_t rtp_session_init(uint8_t session, uint8_t payload_type);

/**
 * @brief Deinitialize RTP session
 *
 * @param session identifier
 * @return esp_err_t ESP_OK on success
 */
esp_err_t rtp_session_deinit(uint8_t session);

/**
 * @brief Set the timestamp increment
 *
 * @param session identifier
 * @param timestamp_inc
 */
void rtp_session_set_timestamp_inc(uint8_t session, size_t timestamp_inc);

/**
 * @brief Get current session state
 *
 * @param session identifier
 * @return rtp_state_t
 */
rtp_state_t rtp_session_get_state(uint8_t session);

/**
 * @brief Create an RTP packet in the `output` buffer.
 *        - The payload is copied into the `output` buffer.
 *        - Both the header and the payload is converted into big endian for network transmission.
 *
 * @param session identifier
 * @param payload buffer with payload
 * @param payload_length length in bytes
 * @param output buffer holding created rtp packet
 * @param size output buffer size
 * @return size_t total rtp packet size in bytes
 */
size_t rtp_create_packet(uint8_t session, void* payload, size_t payload_length, void* output, size_t size);
