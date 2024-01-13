set(srcs)
set(includes_public)
set(includes_private)

list(APPEND srcs
        "src/wifi_manager.c"
        "src/wifi_mgr_credentials.c"
        "src/wifi_mgr_event_handlers.c"
        "src/wifi_mgr_task.c"
        "src/wifi_mgr_provisioning.c"
        )

if(CONFIG_WIFI_MGR_ESP_PROV)
        list(APPEND srcs
                "src/wifi_mgr_esp_prov.c"
        )
endif()

list(APPEND includes_public "include")
list(APPEND includes_private "private")

idf_component_register(SRCS "${srcs}"
        INCLUDE_DIRS "${includes_public}"
        PRIV_INCLUDE_DIRS "${includes_private}"
        REQUIRES esp_wifi wifi_provisioning)
