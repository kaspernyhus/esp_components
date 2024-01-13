/**
 * @file esp_signal_generator.c
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "esp_signal_generator.h"
#include "esp_sig_gen_config.h"

#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/semphr.h"
#include "freertos/task.h"

#include "pink.h"
#include "silence.h"
#include "sine.h"
#include "sine_tab.h"

static const char* TAG = "SIG_GEN";

#ifdef CONFIG_ESP_SIG_GEN_SINE
#define DEFAULT_WAVE ESP_SIG_GEN_WAVE_SINE
#elif CONFIG_ESP_SIG_GEN_SINE_TAB
#define DEFAULT_WAVE ESP_SIG_GEN_WAVE_SINE_TAB
#elif CONFIG_ESP_SIG_GEN_PINK_NOISE
#define DEFAULT_WAVE ESP_SIG_GEN_WAVE_PINK_NOISE
#elif CONFIG_ESP_SIG_GEN_SILENCE
#define DEFAULT_WAVE ESP_SIG_GEN_WAVE_SILENCE
#endif

#ifdef CONFIG_ESP_SIG_GEN_DEF_FREQ
#define DEFAULT_FREQ CONFIG_ESP_SIG_GEN_DEF_FREQ
#else
#define DEFAULT_FREQ 440
#endif
#ifdef CONFIG_ESP_SIG_GEN_DEF_FREQ_CH2
#define DEFAULT_FREQ_CH2 CONFIG_ESP_SIG_GEN_DEF_FREQ_CH2
#else
#define DEFAULT_FREQ_CH2 DEFAULT_FREQ
#endif
static double def_freq[MAX_NUMBER_OF_CHANNELS] = {DEFAULT_FREQ, DEFAULT_FREQ_CH2, DEFAULT_FREQ, DEFAULT_FREQ};

#ifdef CONFIG_ESP_SIG_GEN_NON_INTERLEAVED
#define DEFAULT_LAYOUT ESP_SIG_GEN_LAYOUT_NON_INTERLEAVED
#else
#define DEFAULT_LAYOUT ESP_SIG_GEN_LAYOUT_INTERLEAVED
#endif

#ifdef CONFIG_ESP_SIG_GEN_16
#ifdef CONFIG_ESP_SIG_GEN_BE
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S16BE
#else
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S16LE
#endif
#elif CONFIG_ESP_SIG_GEN_24_3
#ifdef CONFIG_ESP_SIG_GEN_BE
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S24_3BE
#else
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S24_3LE
#endif
#elif CONFIG_ESP_SIG_GEN_24
#ifdef CONFIG_ESP_SIG_GEN_BE
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S24BE
#else
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S24LE
#endif
#else
#ifdef CONFIG_ESP_SIG_GEN_BE
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S32BE
#else
#define DEFAULT_FORMAT ESP_SIG_GEN_AUDIO_FMT_S32LE
#endif
#endif

#ifdef CONFIG_ESP_SIG_GEN_PADDING_LSB
#define DEFAULT_PADDING ESP_SIG_GEN_PADDING_LSB
#else
#define DEFAULT_PADDING ESP_SIG_GEN_PADDING_MSB
#endif

#ifdef CONFIG_ESP_SIG_GEN_SIGN_EXT
#define DEFAULT_SIGN_EXT ESP_SIG_GEN_SIGN_EXT
#else
#define DEFAULT_SIGN_EXT ESP_SIG_GEN_ZERO_EXT
#endif

#ifdef CONFIG_ESP_SIG_GEN_PUSH
#define DEFAULT_METHOD ESP_SIG_GEN_PUSH_METHOD
#else
#define DEFAULT_METHOD ESP_SIG_GEN_PULL_METHOD
#endif

#ifdef CONFIG_ESP_SIG_GEN_PUSH_INTERVAL
#define DEFAULT_PUSH_INTERVAL CONFIG_ESP_SIG_GEN_PUSH_INTERVAL
#else
#define DEFAULT_PUSH_INTERVAL 5
#endif


SemaphoreHandle_t xTimerSem = NULL;
esp_timer_handle_t periodic_timer;

/**
 * @brief Timer callback to drive ESP_SIG_GEN_PUSH_METHOD
 *
 * @param arg
 */
static void _timer_cb(void* arg)
{
    if (xTimerSem != NULL) {
        xSemaphoreGive(xTimerSem);
    }
}

static esp_err_t esp_sig_gen_init_timer(esp_sig_gen_t* sig_gen)
{
    esp_err_t timer_created, timer_started, sem_created, ret = ESP_FAIL;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = _timer_cb,
        .name = "push timer"
    };
    timer_created = esp_timer_create(&periodic_timer_args, &periodic_timer);
    timer_started = esp_timer_start_periodic(periodic_timer, sig_gen->push_interval_ms * 1000);
    xTimerSem = xSemaphoreCreateBinary();
    if (xTimerSem != NULL) {
        sem_created = ESP_OK;
    }
    if ((timer_created == ESP_OK) && (timer_started == ESP_OK) && (sem_created == ESP_OK)) {
        ret = ESP_OK;
    }
    return ret;
}

esp_err_t esp_sig_gen_init(esp_sig_gen_t* sig_gen, esp_sig_gen_config_t* cfg)
{
    assert(sig_gen != NULL);
    assert(cfg != NULL);

    if (!cfg->sample_rate)
        sig_gen->sample_rate = CONFIG_ESP_SIG_GEN_DEF_SAMPLE_RATE;
    else
        sig_gen->sample_rate = cfg->sample_rate;
    if (!cfg->wave)
        sig_gen->wave = DEFAULT_WAVE;
    else
        sig_gen->wave = cfg->wave;
    if (!cfg->freq[0])
        memcpy(sig_gen->freq, def_freq, MAX_NUMBER_OF_CHANNELS*sizeof(double));
    else {
        memcpy(sig_gen->freq, def_freq, MAX_NUMBER_OF_CHANNELS*sizeof(double));
        // if config frequency is not 0 add, else use DEFAULT_FREQ
        for (int i = 0; i < MAX_NUMBER_OF_CHANNELS; i++) {
            if (cfg->freq[i] != 0 )
                sig_gen->freq[i] = cfg->freq[i];
        }
    }
    if (!cfg->volume)
        sig_gen->volume = CONFIG_ESP_SIG_GEN_DEF_VOLUME / 100.0;
    else
        sig_gen->volume = cfg->volume;
    if (!cfg->format)
        sig_gen->format = DEFAULT_FORMAT;
    else
        sig_gen->format = cfg->format;
    if (!cfg->padding)
        sig_gen->padding = DEFAULT_PADDING;
    else
        sig_gen->padding = cfg->padding;
    if (!cfg->sign_extension)
        sig_gen->sign_extension = DEFAULT_SIGN_EXT;
    else
        sig_gen->sign_extension = cfg->sign_extension;
    if (!cfg->layout)
        sig_gen->layout = DEFAULT_LAYOUT;
    else
        sig_gen->layout = cfg->layout;
    if (!cfg->channels)
        sig_gen->channels = CONFIG_ESP_SIG_GEN_CHANNELS;
    else
        sig_gen->channels = cfg->channels;
    if (!cfg->method)
        sig_gen->method = DEFAULT_METHOD;
    else
        sig_gen->method = cfg->method;
    if (!cfg->push_interval_ms)
        sig_gen->push_interval_ms = DEFAULT_PUSH_INTERVAL;
    else
        sig_gen->push_interval_ms = cfg->push_interval_ms;

    // FORMAT
    switch (sig_gen->format) {
    case ESP_SIG_GEN_AUDIO_FMT_S16LE:
        sig_gen->scale = 32767.0;
        sig_gen->bytes_per_sample = 2;
        sig_gen->flip_endianess = false;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S16BE:
        sig_gen->scale = 32767.0;
        sig_gen->bytes_per_sample = 2;
        sig_gen->flip_endianess = true;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S24_3LE:
        sig_gen->scale = 8388607.0;
        sig_gen->bytes_per_sample = 3;
        sig_gen->flip_endianess = false;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S24_3BE:
        sig_gen->scale = 8388607.0;
        sig_gen->bytes_per_sample = 3;
        sig_gen->flip_endianess = true;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S24LE:
        sig_gen->scale = 8388607.0;
        sig_gen->bytes_per_sample = 4;
        sig_gen->flip_endianess = false;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S24BE:
        sig_gen->scale = 8388607.0;
        sig_gen->bytes_per_sample = 4;
        sig_gen->flip_endianess = true;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S32LE:
        sig_gen->scale = 2147483647.0;
        sig_gen->bytes_per_sample = 4;
        sig_gen->flip_endianess = false;
        break;
    case ESP_SIG_GEN_AUDIO_FMT_S32BE:
        sig_gen->scale = 2147483647.0;
        sig_gen->bytes_per_sample = 4;
        sig_gen->flip_endianess = true;
        break;
    default:
        ESP_LOGE(TAG, "Unknown format");
        return ESP_FAIL;
    }

    // WAVEFORM
    switch (sig_gen->wave) {
    case ESP_SIG_GEN_WAVE_SINE:
        esp_sig_gen_sine_init(sig_gen);
        if (sig_gen->bytes_per_sample == 2) {
            sig_gen->process = esp_sig_gen_create_sine_16;
        } else {
            sig_gen->process = esp_sig_gen_create_sine_24_32;
        }
        break;
    case ESP_SIG_GEN_WAVE_SINE_TAB:
        if (esp_sig_gen_sine_tab_init(sig_gen, DEFAULT_TAB_SIZE, USE_VOLUME) == ESP_OK) {
            ESP_LOGI(TAG, "Lookup table initalized");
        };
        if (sig_gen->bytes_per_sample == 2) {
            sig_gen->process = esp_sig_gen_create_sine_tab_16;
        } else {
            sig_gen->process = esp_sig_gen_create_sine_tab_24_32;
        }
        break;
    case ESP_SIG_GEN_WAVE_PINK_NOISE:
        esp_sig_gen_pink_noise_init();
        sig_gen->process = esp_sig_gen_create_pink_16;
        break;
    case ESP_SIG_GEN_WAVE_SILENCE:
        sig_gen->process = esp_sig_gen_create_silence;
        break;
    default:
        ESP_LOGE(TAG, "Unknown waveform");
        return ESP_FAIL;
    }

    // Method
    if (sig_gen->method == ESP_SIG_GEN_PUSH_METHOD) {
        if (sig_gen->push_interval_ms == 0) {
            ESP_LOGW(TAG, "push_interval_ms == 0");
        }
        if (!sig_gen->running) {
            sig_gen->samples_per_callback = ((sig_gen->sample_rate / 1000) * sig_gen->channels) * sig_gen->push_interval_ms;
            if (esp_sig_gen_init_timer(sig_gen) == ESP_OK) {
                sig_gen->running = true;
                ESP_LOGI(TAG, "Periodic audio callback started. Interval: %zu ms / %zu samples", sig_gen->push_interval_ms, sig_gen->samples_per_callback);
            }
        } else {
            ESP_LOGE(TAG, "Audio callback already running");
        }
    }

    // Reset phase angle
    memset(sig_gen->phase_angle, 0.0, 4*sizeof(double));

    ESP_LOGI(TAG, "Signal Generator Initialized\n\
        Waveform: %s\n\
        Frequency: [%.2f, %.2f, %.2f, %.2f]\n\
        Sample Rate: %d\n\
        Channels: %d\n\
        Volume: %.2f\n\
        Format: %s\n\
        Padding: %s\n\
        Sign ext: %s\n\
        Layout: %s\n\
        Method: %s\n",
        WAVE_STRING[sig_gen->wave],
        sig_gen->freq[0],
        sig_gen->freq[1],
        sig_gen->freq[2],
        sig_gen->freq[3],
        sig_gen->sample_rate,
        sig_gen->channels,
        sig_gen->volume,
        FORMAT_STRING[sig_gen->format],
        (sig_gen->bytes_per_sample == 4) && (sig_gen->scale < 2147483647) ? PADDING_STRING[sig_gen->padding] : "-",
        (sig_gen->bytes_per_sample == 4) && (sig_gen->scale < 2147483647) ? SIGNEXT_STRING[sig_gen->sign_extension] : "-",
        LAYOUT_STRING[sig_gen->layout],
        METHOD_STRING[sig_gen->method]);
    return ESP_OK;
}

esp_err_t esp_sig_gen_deinit(esp_sig_gen_t* sig_gen)
{
    esp_err_t ret = ESP_FAIL;
    if (sig_gen->wave_table != NULL) {
        free(sig_gen->wave_table);
        sig_gen->wave_table = NULL;
        ret = ESP_OK;
    }
    return ret;
}

size_t esp_sig_gen_fill(esp_sig_gen_t* sig_gen, void* buffer, size_t size, size_t samples)
{
    assert(sig_gen != NULL);

    /* Check that buffer is large enough to hold requested number of samples */
    if (size < (sig_gen->bytes_per_sample * samples)) {
        samples = size / sig_gen->bytes_per_sample;
        ESP_LOGE(TAG, "Buffer too small. Generating %zu samples", samples);
    }
    return sig_gen->process(sig_gen, buffer, samples);
}

esp_err_t esp_sig_gen_read(esp_sig_gen_t* sig_gen, void* dest, size_t size, size_t* bytes_read, TickType_t ticks_to_wait)
{
    assert(sig_gen != NULL);
    size_t bytes_generated;
    bytes_generated = esp_sig_gen_fill(sig_gen, (uint8_t*)dest, size, sig_gen->samples_per_callback);
    if (xTimerSem != NULL) {
        if (xSemaphoreTake(xTimerSem, ticks_to_wait) != pdTRUE) {
            *bytes_read = 0;
            return ESP_FAIL;
        }
    }
    *bytes_read = bytes_generated;
    return ESP_OK;
}
