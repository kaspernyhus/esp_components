/**
 * @file esp_sig_gen_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_METHOD(METHOD) \
        METHOD(ESP_SIG_GEN_DEFAULT_METHOD) \
        METHOD(ESP_SIG_GEN_PULL_METHOD) \
        METHOD(ESP_SIG_GEN_PUSH_METHOD) \
        METHOD(ESP_SIG_GEN_METHOD_MAX) \

#define FOREACH_FORMAT(FORMAT) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_DEFAULT) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S16LE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S16BE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S24LE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S24BE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S24_3LE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S24_3BE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S32LE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_S32BE) \
        FORMAT(ESP_SIG_GEN_AUDIO_FMT_MAX)

#define FOREACH_PADDING(PADDING) \
        PADDING(ESP_SIG_GEN_PADDING_DEFAULT) \
        PADDING(ESP_SIG_GEN_PADDING_MSB) \
        PADDING(ESP_SIG_GEN_PADDING_LSB) \
        PADDING(ESP_SIG_GEN_PADDING_MAX)

#define FOREACH_SIGNEXT(SIGNEXT) \
        SIGNEXT(ESP_SIG_GEN_SIGN_EXT_DEFAULT) \
        SIGNEXT(ESP_SIG_GEN_ZERO_EXT) \
        SIGNEXT(ESP_SIG_GEN_SIGN_EXT) \
        SIGNEXT(ESP_SIG_GEN_SIGN_MAX)

#define FOREACH_LAYOUT(LAYOUT) \
        LAYOUT(ESP_SIG_GEN_LAYOUT_DEFAULT) \
        LAYOUT(ESP_SIG_GEN_LAYOUT_INTERLEAVED) \
        LAYOUT(ESP_SIG_GEN_LAYOUT_NON_INTERLEAVED) \
        LAYOUT(ESP_SIG_GEN_LAYOUT_MAX)

#define FOREACH_WAVE(WAVE) \
        WAVE(ESP_SIG_GEN_WAVE_DEFAULT) \
        WAVE(ESP_SIG_GEN_WAVE_SINE_TAB) \
        WAVE(ESP_SIG_GEN_WAVE_SINE) \
        WAVE(ESP_SIG_GEN_WAVE_PINK_NOISE) \
        WAVE(ESP_SIG_GEN_WAVE_SILENCE) \
        WAVE(ESP_SIG_GEN_WAVE_MAX)

typedef enum {
    ESP_SIG_GEN_CH_1,
    ESP_SIG_GEN_CH_2,
    ESP_SIG_GEN_CH_3,
    ESP_SIG_GEN_CH_4,
    ESP_SIG_GEN_CH_MAX
} esp_sig_gen_ch_num_t;

typedef enum {
    FOREACH_METHOD(GENERATE_ENUM)
} esp_sig_gen_method_t;

static const char* const METHOD_STRING[] = {
    FOREACH_METHOD(GENERATE_STRING)
};

typedef enum {
    FOREACH_FORMAT(GENERATE_ENUM)
} esp_sig_gen_audio_format_t;

static const char* const FORMAT_STRING[] = {
    FOREACH_FORMAT(GENERATE_STRING)
};

typedef enum {
    FOREACH_PADDING(GENERATE_ENUM)
} esp_sig_gen_padding_t;

static const char* const PADDING_STRING[] = {
    FOREACH_PADDING(GENERATE_STRING)
};

typedef enum {
    FOREACH_SIGNEXT(GENERATE_ENUM)
} esp_sig_gen_sign_ext_t;

static const char* const SIGNEXT_STRING[] = {
    FOREACH_SIGNEXT(GENERATE_STRING)
};

typedef enum {
    FOREACH_LAYOUT(GENERATE_ENUM)
} esp_sig_gen_audio_layout_t;

static const char* const LAYOUT_STRING[] = {
    FOREACH_LAYOUT(GENERATE_STRING)
};

typedef enum {
    FOREACH_WAVE(GENERATE_ENUM)
} esp_sig_gen_audio_wave_t;

static const char* const WAVE_STRING[] = {
    FOREACH_WAVE(GENERATE_STRING)
};
