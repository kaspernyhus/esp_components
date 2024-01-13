/**
 * @file silence.c
 * @author Kasper Nyhus (kanyhus@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "silence.h"
#include "string.h"

size_t esp_sig_gen_create_silence(esp_sig_gen_t* sig_gen, void* buffer, size_t num_samples)
{
    size_t num_bytes = num_samples * sig_gen->bytes_per_sample;
    memset(buffer, 0, num_bytes);
    return num_bytes;
}
