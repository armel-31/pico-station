#BME680 Driver

add_library(bme680 INTERFACE)

target_sources(bme680 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bme680.c
)

target_include_directories(bme680 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(bme680 INTERFACE
    pico_stdlib
    hardware_i2c)

target_compile_options(bme680 INTERFACE -Wno-error=uninitialized)
