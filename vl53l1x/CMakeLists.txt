#VL53l1X Driver

add_library(vl53l1x INTERFACE)

target_sources(vl53l1x INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/vl53l1x.c
)

target_include_directories(vl53l1x INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(vl53l1x INTERFACE
    pico_stdlib
    hardware_i2c)
