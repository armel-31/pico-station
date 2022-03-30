#ST7789 Driver

add_library(st7789 INTERFACE)

target_sources(st7789 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/st7789.c
    ${CMAKE_CURRENT_LIST_DIR}/fonts.c
)

target_include_directories(st7789 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_SOURCE_DIR}/
)

target_link_libraries(st7789 INTERFACE
    pico_stdlib
    hardware_spi)
