# CMakeLists for pico-station
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-station C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add sensors and devices directory
add_subdirectory(hmi/st7789)
add_subdirectory(sensors/sgp30)
add_subdirectory(sensors/bme680)
add_subdirectory(sensors/vl53l1x)

# Create pico-station executable
add_executable(pico-station main.c)

target_link_libraries(pico-station
	sgp30
	st7789
    bme680
	vl53l1x)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico-station 1)
pico_enable_stdio_uart(pico-station 0)

