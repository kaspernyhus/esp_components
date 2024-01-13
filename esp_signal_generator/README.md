# Signal Generator for ESP-IDF

## Waveforms
- Sine
- Sine (using lookup table - much faster generation suited for real time use)
- Pink noise
- Silence

## How to use
```C
esp_sig_gen_t sig_gen;

esp_sig_gen_config_t sig_gen_cfg = {
    .sample_rate = 48000,
    .volume = 1.0,
};
esp_sig_gen_init(&sig_gen, &sig_gen_cfg);
```

Generate 64 samples in a 128 byte user buffer:
```
esp_sig_gen_fill(&sig_gen, buffer, 128, 64);
```

## Default configuration
These parameters can be changed in menuconfig and will populate the parameters not passed in the `esp_sig_gen_config_t` struct to the `esp_sig_gen_init` call.
- Number of channels: 1
- Waveform: Sine
- Frequency: 440
- Volume: 0.8 (0.0-1.0)
- Format: unsigned 16-bit LE
- Layout: interleaved
- Method: pull

Supports 1-4 channels with frequencies from 1Hz-20kHz selectable for each channel.
#### Formats:
- Signed 16bit LE
- Signed 16bit BE
- Signed 24bit LE (packed)
- Signed 24bit BE (packed)
- Signed 24bit LE in 32bit wide samples (zero padded)
- Signed 24bit BE in 32bit wide samples (zero padded)
- Signed 32bit LE
- Signed 32bit BE

#### Layouts: 
- Interleaved channels
- Non-interleaved channels

#### Method:
- pull
- push

`esp_sig_gen_read` is used in push method (Only available for Sine Table Lookup mode). The function blocks for the amount of time set in the config parameter `push_interval_ms`, and cant then be used to push samples downstream.
Note: FreeRTOS probably has to be set to max (100Hz) 

## Notes
- Sine generation is pretty slow and pink noice generation is VERY slow and cannot be used to generate samples in real time only Sine table lookup is fast enough.
