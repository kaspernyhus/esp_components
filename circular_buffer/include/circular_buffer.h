#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


typedef struct {
    uint8_t *buffer;    // Statically allocated user provided buffer
    size_t size;        // Buffer size in bytes
    size_t write;       // Write index
    size_t read;        // Read index
} ringbuf_t;



esp_err_t ringbuf_init(ringbuf_t *rb, uint8_t *buffer, size_t size);
void ringbuf_reset(ringbuf_t *rb);
uint8_t ringbuf_full(ringbuf_t *rb);

size_t ringbuf_write(ringbuf_t *rb, void *data, size_t size);
size_t ringbuf_read(ringbuf_t *rb, void *data, size_t bytes);

void ringbuf_print_info(const ringbuf_t *rb);
void ringbuf_print(const ringbuf_t *rb);