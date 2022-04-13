# CMakeLists for pico-sim7920e
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-sim7020e C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Create pico-station executable
add_executable(pico-sim7020e sim7020e.c)

target_link_libraries(pico-sim7020e
    pico_stdlib
    hardware_spi
    hardware_uart)


# enable usb output, disable uart output
pico_enable_stdio_usb(pico-wireless 1)
pico_enable_stdio_uart(pico-wireless 0)

