/**
 * @file sine.c
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sine.h"
#include "common.h"
#include "esp_sig_gen_config.h"
#include <string.h>

void esp_sig_gen_sine_init(esp_sig_gen_t* sig_gen)
{
    sig_gen->two_pi_over_sr = M_2PI / sig_gen->sample_rate;
    memset(sig_gen->phase_angle, 0.0, 4*sizeof(double));
}

size_t esp_sig_gen_create_sine_16(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples)
{
    int channel_step, sample_step;
    uint16_t* ptr;
    size_t samples = 0;

    size_t number_of_samples_per_channel = num_samples / sig_gen->channels;

    if (sig_gen->layout == ESP_SIG_GEN_LAYOUT_INTERLEAVED) {
        channel_step = 1;
        sample_step = sig_gen->channels * sig_gen->bytes_per_sample;
    } else {
        channel_step = number_of_samples_per_channel;
        sample_step = sig_gen->bytes_per_sample;
    }

    double step[4];
    for (int i = 0; i < 4; i++) {
        step[i] = sig_gen->freq[i] * sig_gen->two_pi_over_sr;
    }
    double amp = sig_gen->volume * sig_gen->scale;

    for (int i = 0; i < number_of_samples_per_channel; i++) {
        ptr = (uint16_t*)buffer;
        for (int channel = 0; channel < sig_gen->channels; ++channel) {
            sig_gen->phase_angle[channel] += step[channel];
            if (sig_gen->phase_angle[channel] >= M_2PI) { // wrap phase every 2pi
                sig_gen->phase_angle[channel] -= M_2PI;
            }
            uint16_t sample = (uint16_t)(sin(sig_gen->phase_angle[channel]) * amp);
            if (!sig_gen->flip_endianess) {
                *ptr = sample;
            } else {
                *ptr = (sample >> 8) | (sample << 8);
            }
            ptr += channel_step;
            samples++;
        }
        buffer += sample_step;
    }
    return samples * sig_gen->bytes_per_sample;
}

size_t esp_sig_gen_create_sine_24_32(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples)
{
    int channel_step, sample_step;
    uint8_t* ptr;
    size_t samples = 0;

    size_t number_of_samples_per_channel = num_samples / sig_gen->channels;

    if (sig_gen->layout == ESP_SIG_GEN_LAYOUT_INTERLEAVED) {
        channel_step = 1;
        sample_step = sig_gen->channels * sig_gen->bytes_per_sample;
    } else {
        channel_step = number_of_samples_per_channel;
        sample_step = 1;
    }

    double step[4];
    for (int i = 0; i < 4; i++) {
        step[i] = sig_gen->freq[i] * sig_gen->two_pi_over_sr;
    }
    double amp = sig_gen->volume * sig_gen->scale;

    for (int i = 0; i < number_of_samples_per_channel; i++) {
        ptr = (uint8_t*)buffer;
        for (int channel = 0; channel < sig_gen->channels; ++channel) {
            sig_gen->phase_angle[channel] += step[channel];
            if (sig_gen->phase_angle[channel] >= M_2PI) { // wrap phase every 2pi
                sig_gen->phase_angle[channel] -= M_2PI;
            }

            uint32_t sample = (uint32_t)(sin(sig_gen->phase_angle[channel]) * amp);
            switch (sig_gen->format) {
            case ESP_SIG_GEN_AUDIO_FMT_S24_3LE:
                *ptr++ = (sample >> 0 & 0xff);
                *ptr++ = (sample >> 8 & 0xff);
                *ptr   = (sample >> 16 & 0xff);
                break;
            case ESP_SIG_GEN_AUDIO_FMT_S24_3BE:
                *ptr++ = (sample >> 16 & 0xff);
                *ptr++ = (sample >> 8 & 0xff);
                *ptr   = (sample >> 0 & 0xff);
                break;
            case ESP_SIG_GEN_AUDIO_FMT_S24LE:
                if (sig_gen->padding == ESP_SIG_GEN_PADDING_LSB) {
                    if (sig_gen->sign_extension == ESP_SIG_GEN_SIGN_EXT) {
                        if ((sample >> 23) & 0x01) {
                            *ptr++ = 0xff;
                        } else {
                            *ptr++ = 0x00;
                        }
                    } else {
                        *ptr++ = 0x00;
                    }
                    *ptr++ = (sample >> 0 & 0xff);
                    *ptr++ = (sample >> 8 & 0xff);
                    *ptr   = (sample >> 16 & 0xff);
                } else {
                    *ptr++ = (sample >> 0 & 0xff);
                    *ptr++ = (sample >> 8 & 0xff);
                    *ptr++ = (sample >> 16 & 0xff);
                    if (sig_gen->sign_extension == ESP_SIG_GEN_SIGN_EXT) {
                        if ((sample >> 23) & 0x01) {
                            *ptr = 0xff;
                        } else {
                            *ptr = 0x00;
                        }
                    } else {
                        *ptr = 0x00;
                    }
                }
                break;
            case ESP_SIG_GEN_AUDIO_FMT_S24BE:
                if (sig_gen->padding == ESP_SIG_GEN_PADDING_LSB) {
                    if (sig_gen->sign_extension == ESP_SIG_GEN_SIGN_EXT) {
                        if ((sample >> 23) & 0x01) {
                            *ptr++ = 0xff;
                        } else {
                            *ptr++ = 0x00;
                        }
                    } else {
                        *ptr++ = 0x00;
                    }
                    *ptr++ = (sample >> 16 & 0xff);
                    *ptr++ = (sample >> 8 & 0xff);
                    *ptr   = (sample >> 0 & 0xff);
                } else {
                    *ptr++ = (sample >> 16 & 0xff);
                    *ptr++ = (sample >> 8 & 0xff);
                    *ptr++ = (sample >> 0 & 0xff);
                    if (sig_gen->sign_extension == ESP_SIG_GEN_SIGN_EXT) {
                        if ((sample >> 23) & 0x01) {
                            *ptr = 0xff;
                        } else {
                            *ptr = 0x00;
                        }
                    } else {
                        *ptr = 0x00;
                    }
                }
                break;
            case ESP_SIG_GEN_AUDIO_FMT_S32LE:
                *ptr++ = (sample >> 0) & 0xff;
                *ptr++ = (sample >> 8) & 0xff;
                *ptr++ = (sample >> 16) & 0xff;
                *ptr   = (sample >> 24) & 0xff;
                break;
            case ESP_SIG_GEN_AUDIO_FMT_S32BE:
                *ptr++ = (sample >> 24) & 0xff;
                *ptr++ = (sample >> 16) & 0xff;
                *ptr++ = (sample >> 8) & 0xff;
                *ptr   = (sample >> 0) & 0xff;
                break;
            default:
                break;
            }
            ptr += channel_step;
            samples++;
        }
        buffer += sample_step;
    }
    return samples * sig_gen->bytes_per_sample;
}
