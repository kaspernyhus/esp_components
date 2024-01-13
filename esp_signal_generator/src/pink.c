/**
 * @file pink.c
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "pink.h"
#include "esp_random.h"

#define PINK_MAX_RANDOM_ROWS (30)
#define PINK_RANDOM_BITS (16)
#define PINK_RANDOM_SHIFT ((sizeof(long) * 8) - PINK_RANDOM_BITS)

typedef struct {
    int32_t rows[PINK_MAX_RANDOM_ROWS];
    int32_t running_sum; /* Used to optimize summing of generators. */
    int index; /* Incremented each sample. */
    int index_mask; /* Index wrapped by ANDing with this mask. */
    double scalar; /* Used to scale within range of -1.0 to +1.0 */
} pink_noise_t;

pink_noise_t pink;

void esp_sig_gen_pink_noise_init(void)
{
    int num_rows = 12; /* arbitrary: 1 .. PINK_MAX_RANDOM_ROWS */
    int32_t pmax;

    pink.index = 0;
    pink.index_mask = (1 << num_rows) - 1;
    /* calculate maximum possible signed random value.
     * Extra 1 for white noise always added. */
    pmax = (num_rows + 1) * (1 << (PINK_RANDOM_BITS - 1));
    pink.scalar = 1.0f / pmax;
    /* Initialize rows. */
    for (int i = 0; i < num_rows; i++)
        pink.rows[i] = 0;
    pink.running_sum = 0;
}

static double esp_sig_gen_generate_pink_noise_value(void)
{
    int32_t new_random;
    int32_t sum;

    /* Increment and mask index. */
    pink.index = (pink.index + 1) & pink.index_mask;

    /* If index is zero, don't update any random values. */
    if (pink.index != 0) {
        /* Determine how many trailing zeros in PinkIndex. */
        /* This algorithm will hang if n==0 so test first. */
        int num_zeros = 0;
        int n = pink.index;

        while ((n & 1) == 0) {
            n = n >> 1;
            num_zeros++;
        }

        /* Replace the indexed ROWS random value.
         * Subtract and add back to RunningSum instead of adding all the random
         * values together. Only one changes each time. */
        pink.running_sum -= pink.rows[num_zeros];
        new_random = 32768.0 - (65536.0 * esp_random() / (UINT32_MAX + 1.0));
        pink.running_sum += new_random;
        pink.rows[num_zeros] = new_random;
    }

    /* Add extra white noise value. */
    new_random = 32768.0 - (65536.0 * esp_random() / (UINT32_MAX + 1.0));
    sum = pink.running_sum + new_random;

    /* Scale to range of -1.0 to 0.9999. */
    double sample = pink.scalar * sum;
    return sample;
}

size_t esp_sig_gen_create_pink_16(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples)
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

    double amp = sig_gen->volume * sig_gen->scale;

    for (int i = 0; i < number_of_samples_per_channel; i++) {
        ptr = (uint16_t*)buffer;
        for (int channel = 0; channel < sig_gen->channels; ++channel) {
            // TODO: handle endianess & bitdepth
            *ptr = (uint16_t)(esp_sig_gen_generate_pink_noise_value() * amp);
            ptr += channel_step;
            samples++;
        }
        buffer += sample_step;
    }
    return samples * sig_gen->bytes_per_sample;
}
