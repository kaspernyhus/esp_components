set(srcs)
set(includes_public)
set(includes_private)
set(compile_options)

if(CONFIG_ENABLE_ESP_SIGNAL_GENERATOR)
        list(APPEND srcs
                "src/sine.c"
                "src/sine_tab.c"
                "src/silence.c"
                "src/pink.c"
                "src/esp_signal_generator.c"
        )

        list(APPEND include_dirs "include")

        list(APPEND priv_include_dirs "private")

        list(APPEND compile_options
                "-O2"
                "-ffast-math"
                "-funroll-loops"
                )

endif()

idf_component_register(SRCS "${srcs}"
            INCLUDE_DIRS "${include_dirs}"
            PRIV_INCLUDE_DIRS "${priv_include_dirs}"
            REQUIRES esp_timer)


if(CONFIG_ENABLE_ESP_SIGNAL_GENERATOR)
        component_compile_options(${compile_options})
endif()
