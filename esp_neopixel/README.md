# ESP Neopixel
Easy to use interface for a single neopixel using `led_strip` component from either Espressif (only new RMT peripheral driver) or adopted version that enables using the old RMT driver.

Using old RMT driver by default. To use new driver `idf_component.yml` dependency must be changed to point to Espressif's [version](https://components.espressif.com/components/espressif/led_strip) in [IDF Component Registry](https://components.espressif.com/).

> NOTE: Kconfig options not implemented!

## How to use
see examples/
```
esp_neopixel_config_t cfg = {
        .rmt_channel = 0,
        .gpio = 15,
        .invert = true
    };
    esp_neopixel_init(&cfg);
```

