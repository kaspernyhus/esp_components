# esp_liblc3
> [!WARNING]
> Highly experimental!

ESP-IDF component to enable LC3 encoding/decoding for audio applications

Using: [liblc3](https://github.com/google/liblc3)


### Example
```
esp_lc3_encoder_t encoder;
esp_lc3_decoder_t decoder;

esp_lc3_config_t lc3_session = {
    .format = LC3_PCM_FORMAT_S24,
    .sample_rate = 48000,
    .frame_size = 10000,
    .nbytes_target = TARGET_NBYTES,
    .stride = 2,
};
esp_lc3_encoder_init(&encoder, &lc3_session);
esp_lc3_decoder_init(&decoder, &lc3_session);

// Encode
esp_lc3_encode(&encoder, in, net_buf);

// Decode
esp_lc3_decode(&decoder, net_buf, out);
```
