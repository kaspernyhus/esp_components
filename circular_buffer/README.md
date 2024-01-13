# A Simple Ringbuffer

Underlying buffer is statically allocated and provided by the user. \
Write and read functions return how many bytes has actually been written/read. \
The buffer wraps around and writes/reads in two chunks under the hood.
> NOTE: Provided buffer size must be an even number of bytes !

Ringbuffer will detect overflow and underflow and print an error message.

## Example
```
#define R_BUF_SZ 192*10   
uint8_t rbuffer[R_BUF_SZ];  
ringbuf_t rbuf;

/* Create ringbuffer */
ringbuf_init(&rbuf,rbuffer,R_BUF_SZ);


/* Put samples to ringbuffer */
size_t bytes_written = ringbuf_write(&rbuf,i2s_buffer,i2s_bytes_read);
if(bytes_written != i2s_bytes_read) {
    ESP_LOGE("ringbuffer", "Ringbuf failed to write enough");
}


/* Get samples from ringbuffer */
size_t bytes_recieved = ringbuf_read(&rbuf,to_usb,CFG_TUD_AUDIO_EP_SZ_IN);
if(bytes_recieved != CFG_TUD_AUDIO_EP_SZ_IN) {
    ESP_LOGE("ringbuffer", "Ringbuf failed to read enough");
}
```