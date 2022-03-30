#SGP30 Driver

add_library(sgp30 INTERFACE)

target_sources(sgp30 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sgp30.c
)

target_include_directories(sgp30 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_SOURCE_DIR}/
)

target_link_libraries(sgp30 INTERFACE
    pico_stdlib
    hardware_i2c)
