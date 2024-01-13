set(srcs)
set(includes)
set(compile_options)

list(APPEND includes
        "include"
        "lib/liblc3/include"
        "lib/liblc3/src"
        )

list(APPEND srcs
        "esp_liblc3.c"
        "lib/liblc3/src/attdet.c"
        "lib/liblc3/src/bits.c"
        "lib/liblc3/src/bwdet.c"
        "lib/liblc3/src/energy.c"
        "lib/liblc3/src/lc3.c"
        "lib/liblc3/src/ltpf.c"
        "lib/liblc3/src/mdct.c"
        "lib/liblc3/src/plc.c"
        "lib/liblc3/src/sns.c"
        "lib/liblc3/src/spec.c"
        "lib/liblc3/src/tables.c"
        "lib/liblc3/src/tns.c"
        )

list(APPEND compile_options
        "-O2"
        "-ffast-math"
        )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes})

component_compile_options(${compile_options})
