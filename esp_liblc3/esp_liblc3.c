/**
 * @file esp_liblc3.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-05-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "esp_log.h"

#include "esp_liblc3.h"
#include "lc3.h"

static const char* TAG = "LC3";

esp_err_t esp_lc3_encoder_init(esp_lc3_encoder_t* enc, const esp_lc3_config_t* cfg)
{
    // TODO: check cfg struct is valid
    enc->format = cfg->format;
    enc->sr_hz = cfg->sample_rate;
    enc->dt_us = cfg->frame_size;
    enc->nbytes_target = cfg->nbytes_target;
    enc->stride = cfg->stride;

    unsigned int enc_size = lc3_encoder_size(enc->dt_us, enc->sr_hz);
    enc->num_frames = lc3_frame_samples(enc->dt_us, enc->sr_hz);

    enc->lc3_encoder_memory = malloc(enc_size);
    if (enc->lc3_encoder_memory == NULL) {
        ESP_LOGE(TAG, "Malloc error");
        return ESP_FAIL;
    }

    enc->encoder = lc3_setup_encoder(enc->dt_us, enc->sr_hz, 0, enc->lc3_encoder_memory);

    if (enc->encoder != NULL) {
        ESP_LOGI(TAG, "LC3 encoder initialized. Num_frames: %d", enc->num_frames);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_lc3_decoder_init(esp_lc3_decoder_t* dec, const esp_lc3_config_t* cfg)
{
    // TODO: check cfg struct is valid
    dec->format = cfg->format;
    dec->sr_hz = cfg->sample_rate;
    dec->dt_us = cfg->frame_size;
    dec->nbytes_target = cfg->nbytes_target;
    dec->stride = cfg->stride;

    unsigned int dec_size = lc3_decoder_size(dec->dt_us, dec->sr_hz);
    dec->num_frames = lc3_frame_samples(dec->dt_us, dec->sr_hz);

    dec->lc3_decoder_memory = malloc(dec_size);
    if (dec->lc3_decoder_memory == NULL) {
        ESP_LOGE(TAG, "Malloc error");
        return ESP_FAIL;
    }

    dec->decoder = lc3_setup_decoder(dec->dt_us, dec->sr_hz, 0, dec->lc3_decoder_memory);

    if (dec->decoder != NULL) {
        ESP_LOGI(TAG, "LC3 decoder initialized. Num_frames: %d", dec->num_frames);
        return ESP_FAIL;
    }

    return ESP_OK;
}


esp_err_t esp_lc3_encode(esp_lc3_encoder_t* enc, const void *in, void *out)
{
    if (lc3_encode(enc->encoder, enc->format, in, enc->stride, enc->nbytes_target, out) != 0) {
        ESP_LOGE(TAG, "Encoding error: Wrong parameters");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_lc3_decode(esp_lc3_decoder_t* dec, const void *in, void *out)
{
    int res = lc3_decode(dec->decoder, in, dec->nbytes_target, dec->format, out, dec->stride);
    if (res != 0) {
        if (res == 1) {
            ESP_LOGW(TAG, "PLC operated");
        } else {
            ESP_LOGE(TAG, "Decoding error: Wrong parameters");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}
