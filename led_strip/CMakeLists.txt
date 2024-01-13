include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(srcs "src/led_strip_api.c")
list(APPEND srcs "src/led_strip_rmt_dev_idf4.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" "interface"
                       REQUIRES "driver")
