/**
 * @file esp_liblc3.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-05-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lc3.h"

#include "esp_err.h"

typedef struct {
    uint8_t* lc3_encoder_memory;
    lc3_encoder_t encoder;
    enum lc3_pcm_format format;
    int sr_hz;
    int dt_us;
    int nbytes_target;
    int stride;
    int num_frames;
} esp_lc3_encoder_t;

typedef struct {
    uint8_t* lc3_decoder_memory;
    lc3_decoder_t decoder;
    enum lc3_pcm_format format;
    int sr_hz;
    int dt_us;
    int nbytes_target;
    int stride;
    int num_frames;
} esp_lc3_decoder_t;

typedef struct {
    enum lc3_pcm_format format;
    int sample_rate;
    int frame_size;
    int nbytes_target;
    int stride;
} esp_lc3_config_t;

/**
 * @brief Initialize encoder
 *
 * @param enc encoder instance
 * @param cfg session configuration
 * @return esp_err_t
 */
esp_err_t esp_lc3_encoder_init(esp_lc3_encoder_t* enc, const esp_lc3_config_t* cfg);

/**
 * @brief Initialize decoder
 *
 * @param dec decoder instance
 * @param cfg session configuration
 * @return esp_err_t
 */
esp_err_t esp_lc3_decoder_init(esp_lc3_decoder_t* dec, const esp_lc3_config_t* cfg);

/**
 * @brief Encode PCM data
 *
 * @param enc encoder instance
 * @param in PCM data
 * @param out encoded data
 * @return esp_err_t
 */
esp_err_t esp_lc3_encode(esp_lc3_encoder_t* enc, const void *in, void *out);

/**
 * @brief Decode LC3 encoded data
 *
 * @param dec decoder instance
 * @param in encoded data
 * @param out PCM data
 * @return esp_err_t
 */
esp_err_t esp_lc3_decode(esp_lc3_decoder_t* dec, const void *in, void *out);
