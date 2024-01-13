#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef struct
{
    uint32_t reg;
    uint32_t timestamp;
    char *tag;
} log_reg_t;


typedef struct
{
    log_reg_t *buffer;
    size_t size;
    size_t write;
    uint8_t is_printed;
    uint8_t incl_time;
    char *tag;
} log_reg_buffer_t;


typedef struct
{
    uint8_t *buffer;
    size_t size;
    size_t write;
    uint8_t is_printed;
    size_t delayed_start;
    char *tag;
} log_buffer_t;


/* Global log buffer */
log_buffer_t global_log_buf;
log_reg_buffer_t global_reg_buf;


void log_buffer_init(log_buffer_t *tb, uint8_t *buffer, size_t size, size_t delayed_start, char *tag);
void log_buffer_add(log_buffer_t *tb, void *data, size_t bytes);
void log_buffer_add_byte(log_buffer_t *tb, uint8_t data);
void log_buffer_enable_global(uint8_t *buffer, size_t size, size_t delayed_start);
void log_buffer_print(log_buffer_t *tb);

void log_reg_buffer_init(log_reg_buffer_t *lr, log_reg_t *buffer, size_t size, uint8_t incl_timestamps, char *tag);
void log_reg_buffer_add(log_reg_buffer_t *lr, uint32_t reg, char *tag);
void log_reg_buffer_enable_global(log_reg_t *buffer, size_t size, uint8_t incl_timestamps);
