/**
 * @file esp_audio_buffer.c
 * @author Kasper Nyhus
 * @brief 
 * @version 0.1
 * @date 2024-02-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "esp_audio_buffer.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "esp_audio_buffer";

esp_err_t esp_audio_buffer_create(esp_audio_buffer_t *buffer, uint32_t size)
{
    if (size & (size - 1))
    {
        ESP_LOGE(TAG, "Size must be a power of 2");
        return ESP_FAIL;
    }

    buffer->_size = size;
    buffer->_mask = size - 1;
    buffer->_data = (uint8_t *)malloc(size * sizeof(uint8_t));

    if (buffer->_data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer");
        return ESP_FAIL;
    }

    buffer->_nwr = 0;
    buffer->_nrd = 0;

    return ESP_OK;
}

esp_err_t esp_audio_buffer_create_static(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t size)
{
    if (size & (size - 1))
    {
        ESP_LOGE(TAG, "Size must be a power of 2");
        return ESP_FAIL;
    }

    buffer->_size = size;
    buffer->_mask = size - 1;
    buffer->_data = data;

    buffer->_nwr = 0;
    buffer->_nrd = 0;

    return ESP_OK;
}

esp_err_t esp_audio_buffer_destroy(esp_audio_buffer_t *buffer)
{
    if (buffer->_data == NULL)
    {
        ESP_LOGE(TAG, "Buffer has already been destroyed");
        return ESP_FAIL;
    }
    free(buffer->_data);
    buffer->_data = NULL;
    return ESP_OK;
}

void esp_audio_buffer_reset(esp_audio_buffer_t *buffer)
{
    buffer->_nwr = 0;
    buffer->_nrd = 0;
    memset(buffer->_data, 0, buffer->_size);
}

esp_err_t esp_audio_buffer_write(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t len)
{
    if (len > esp_audio_buffer_wr_avail(buffer))
    {
        ESP_LOGE(TAG, "Not enough space in buffer");
        return ESP_FAIL;
    }

    uint8_t* wr_ptr = esp_audio_buffer_wr_ptr(buffer);
    uint32_t linavail = esp_audio_buffer_wr_linavail(buffer);
    if (linavail >= len) { // Can be written in one go
        memcpy(wr_ptr, data, len);
    } else {
        memcpy(wr_ptr, data, linavail); // Write first part from write pointer to end of buffer
        memcpy(buffer->_data, &data[linavail], len - linavail); // Write second part from start of buffer to end of data
    }

    esp_audio_buffer_wr_commit(buffer, len);
    return ESP_OK;
}

esp_err_t esp_audio_buffer_read(esp_audio_buffer_t *buffer, uint8_t *data, uint32_t len)
{
    if (len > esp_audio_buffer_rd_avail(buffer))
    {
        ESP_LOGE(TAG, "Not enough data in buffer");
        return ESP_FAIL;
    }

    uint8_t* rd_ptr = esp_audio_buffer_rd_ptr(buffer);
    uint32_t linavail = esp_audio_buffer_rd_linavail(buffer);
    if (linavail >= len) { // Can be read in one go
        memcpy(data, rd_ptr, len);
    } else {
        memcpy(data, rd_ptr, linavail); // Read first part from read pointer to end of buffer
        memcpy(&data[linavail], buffer->_data, len - linavail); // Read second part from start of buffer to end of data
    }

    esp_audio_buffer_rd_commit(buffer, len);
    return ESP_OK;
}
