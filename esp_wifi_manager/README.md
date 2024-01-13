# Esp WiFi Manager

This component handles WiFi connection related tasks such as initializing the WiFi subsystem to be in STATION mode, handling WiFi credentials and provisioning and handle when a connection is lost.

The component uses the `esp_event` system to communicate events to application code. Subscribing to these event is done by passing an event handler like this:
```
esp_event_handler_instance_register(WIFI_MGR_EVENT, ESP_EVENT_ANY_ID, &wifi_mgr_event_handler, NULL, NULL);
```


## Provisioning
There are three methods for provisioning a device (menuconfig):
- hardcoded credentials
- Esp BLE Prov (App)
- Custom provisioning using the `wifi_mgr_prov_t` interface

### Hardcoded credentials
are compiled into the firmware. The option 'Force new credentials' overwrites the flash on every boot ensuring that the hardcoded credentials are used.

### Esp BLE Prov
Uses the Espressif App for iPhone/Android to provision the device

### Custom Provisioning
Use the interface defined in `wifi_mgr_provisioning.h` to make the WiFi Manager able to call into a custom provisioning scheme

## Example using custom provisioning
```
/* Initialize NVS flash for storing credentials */
    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        status = nvs_flash_init();
        ESP_ERROR_CHECK(status);
    }

    /* Create the default event loop used for sending event notifications */
    esp_event_loop_create_default();

    /* Start WiFi Manager task and wait a little to make sure it is running */
    xTaskCreate(wifi_mgr_task, "WiFi Manager Task", 4096, NULL, 2, NULL);
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Initialize network interface */
    esp_netif_init();

    /* Subscribe to events from WiFi Manager */
    esp_event_handler_instance_register(WIFI_MGR_EVENT, ESP_EVENT_ANY_ID, &wifi_mgr_event_handler, NULL, NULL);

    /* Start WiFi Manager */
    wifi_mgr_init_station();
    wifi_mgr_start();
```
