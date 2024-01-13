# LED Strip Driver
Kasper Nyhus 03/01/24

Led strip driver adobted from Espressif official implementation [here](https://components.espressif.com/components/espressif/led_strip)

## Notes
Enable usage of the depricated RMT peripheral driver in idf versions 5.0 and above, which is not possible in the new led_strip implementation. Everything else has been stripped off.

For support for new RMT peripheral driver the official led strip driver should be used.
