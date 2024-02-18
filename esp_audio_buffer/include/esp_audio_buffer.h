/**
 * @file esp_audio_buffer.h
 * @author Kasper Nyhus
 * @brief 
 * @version 0.1
 * @date 2024-02-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"

typedef struct {
    uint8_t *_data;
    uint32_t _size;
    uint32_t _mask;
    uint32_t _nwr;
    uint32_t _nrd;
} esp_audio_buffer_t;

esp_err_t esp_audio_buffer_create(esp_audio_buffer_t *buffer, uint32_t size);
esp_err_t esp_audio_buffer_create_static(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t size);
esp_err_t esp_audio_buffer_destroy(esp_audio_buffer_t *buffer);

void esp_audio_buffer_reset(esp_audio_buffer_t *buffer);
esp_err_t esp_audio_buffer_write(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t len);
esp_err_t esp_audio_buffer_read(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t len);

static inline uint32_t esp_audio_buffer_wr_avail(esp_audio_buffer_t *buffer) {return buffer->_size - buffer->_nwr + buffer->_nrd;};
static inline uint32_t esp_audio_buffer_wr_linavail(esp_audio_buffer_t *buffer) { return buffer->_size - (buffer->_nwr & buffer->_mask);};
static inline uint32_t esp_audio_buffer_rd_avail(esp_audio_buffer_t *buffer) {return buffer->_nwr - buffer->_nrd;};
static inline uint32_t esp_audio_buffer_rd_linavail(esp_audio_buffer_t *buffer) {return buffer->_nwr - (buffer->_nrd & buffer->_mask);};

static inline uint8_t *esp_audio_buffer_wr_ptr(esp_audio_buffer_t *buffer) {return &buffer->_data[buffer->_nwr & buffer->_mask];};
static inline uint8_t *esp_audio_buffer_rd_ptr(esp_audio_buffer_t *buffer) {return &buffer->_data[buffer->_nrd & buffer->_mask];};

static inline void esp_audio_buffer_wr_commit(esp_audio_buffer_t *buffer, uint32_t len) {buffer->_nwr += len;};
static inline void esp_audio_buffer_rd_commit(esp_audio_buffer_t *buffer, uint32_t len) {buffer->_nrd += len;};
