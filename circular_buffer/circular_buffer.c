#include "circular_buffer.h"
#include "esp_log.h"
#include "esp_err.h"

const char *RB_TAG = "ringbuffer";
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

/*
Initialize ringbuffer
@rb: ringbuffer obj
@buffer: user provided buffer
@size: size of user buffer in bytes
*/
esp_err_t ringbuf_init(ringbuf_t *rb, uint8_t *buffer, size_t size)
{
    if(size % 2 != 0) {
        ESP_LOGE(RB_TAG,"Please provide a buffer with an even number of bytes");
        return pdFAIL;
    }
    
    rb->buffer = buffer;
    rb->size = size;
    ringbuf_reset(rb);
    ESP_LOGI(RB_TAG,"Ringbuffer created. Size: %d bytes", rb->size);
    return pdTRUE;
}


void ringbuf_reset(ringbuf_t *rb)
{
    memset(rb->buffer,0,rb->size);
    rb->write = 0;
    rb->read = 0;
}


/*
Writes to a ringbuffer obj
@data: data to be written
@bytes: number of bytes to be written
return: number of actual bytes written
*/
size_t ringbuf_write(ringbuf_t *rb, void *data, size_t bytes)
{
    if(bytes == 0)
        return 0;
    if(bytes > rb->size) {
        ESP_LOGE(RB_TAG,"ERROR: Trying to write more bytes than size of buffer");
        return 0;
    }
    size_t available = (rb->read > rb->write) ? (rb->read - rb->write) : rb->size - (rb->write - rb->read);
    if(available < bytes)
        ESP_LOGE(RB_TAG,"ERROR: Ringbuffer overflow");

    portENTER_CRITICAL(&spinlock);
    size_t bytes_written = 0;
    // too big to fit, write two blocks
    if(rb->write + bytes > rb->size) {
        size_t first_block = rb->size - rb->write;
        memcpy(rb->buffer + rb->write,data,first_block);
        size_t second_block = bytes - first_block;
        memcpy(rb->buffer,data + first_block,second_block);
        rb->write = second_block;
        bytes_written = first_block + second_block;
    }
    // write all data
    else {
        memcpy(rb->buffer + rb->write,data,bytes);
        rb->write += bytes;
        if(rb->write == rb->size)
            rb->write = 0;
        bytes_written = bytes;
    }
    portEXIT_CRITICAL(&spinlock);
    
    return bytes_written;
}


/*
Reads from a ringbuffer obj
@data: data to be read to
@bytes: number of bytes to be read
return: number of actual bytes read
*/
size_t ringbuf_read(ringbuf_t *rb, void *data, size_t bytes)
{
    if(bytes == 0)
        return 0;
    if(bytes > rb->size) {
        ESP_LOGE(RB_TAG,"ERROR: Trying to read more bytes than size of buffer");
        return 0;
    }
    size_t ready = (rb->read > rb->write) ? (rb->read - rb->write) : rb->size - (rb->write - rb->read);
    if(ready < bytes)
        ESP_LOGE(RB_TAG,"ERROR: Ringbuffer underflow");


    portENTER_CRITICAL(&spinlock);
    size_t bytes_read = 0;
    // wrap around read
    if(rb->read + bytes > rb->size) {
        size_t first_block = rb->size - rb->read;
        memcpy(data,rb->buffer + rb->read,first_block);
        size_t second_block = bytes - first_block;
        memcpy(data + first_block,rb->buffer,second_block);
        rb->read = second_block;
        bytes_read = first_block + second_block;
    }
    // read all data out in one go
    else {
        memcpy(data,rb->buffer + rb->read,bytes);
        rb->read += bytes;
        if(rb->read == rb->size)
            rb->read = 0;
        bytes_read = bytes;
    }
    portEXIT_CRITICAL(&spinlock);
    return bytes_read;
}

/*
Returns true if ringbuffer is full (or empty...)
*/
uint8_t ringbuf_full(ringbuf_t *rb)
{
    return rb->read == rb->write;
}


/*
Prints the Write and Read index
*/
void ringbuf_print_info(const ringbuf_t *rb)
{
    ESP_LOGI("","-----------------------------------------------");
    ESP_LOGI("","Ringbuffer -- Write: %d, Read: %d", rb->write, rb->read);
    ESP_LOGI("","-----------------------------------------------");
}

/*
Prints the contents of the ringbuffer
*/
void ringbuf_print(const ringbuf_t *rb)
{
    ringbuf_print_info(rb);
    ESP_LOG_BUFFER_HEX("",rb->buffer,rb->size);
}