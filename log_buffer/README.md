# Log Buffer

Logs data to a buffer and prints the contents of the buffer when buffer is filled.\
There is two kinds of log buffers:

### log_buffer
"Add bytes to the buffer"\
has delayed start option
### reg_buffer
"Log a 32bit variable tag for each entry"\
has timestamp add option

### Global scope
log_buffer object: `global_log_buf;`\
log_reg_buffer object: `log_reg_buffer_t global_reg_buf;`

initialization: `void log_buffer_enable_global(*buffer, size, delayed_start);`\
initialization: `log_reg_buffer_enable_global(*buffer, size, incl_timestamps);`


## Example
```
#define LOG_BUF_SZ 1000
uint8_t logbuffer[LOG_BUF_SZ];
log_buffer_t logbuf;

log_buffer_init(&logbuf,logbuffer,LOG_BUF_SZ,1000,"SigGen data");

...

log_buffer_add(&logbuf,to_usb,CFG_TUD_AUDIO_EP_SZ_IN);

...

I (356) : -------------------------------------------------
I (356) : SigGen data
I (356) : -------------------------------------------------
I (356) log_buffer: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
I (366) log_buffer: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
I (366) log_buffer: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
I (376) log_buffer: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
I (386) log_buffer: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
I (396) log_buffer: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
I (396) log_buffer: 60 61 62 63 


```