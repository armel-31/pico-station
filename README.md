# Pico-Station

The aim of this project is to propose open code in C for the [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html), to integrate several environnemental sensors into your embedded systems ([Pimoroni breakouts](https://shop.pimoroni.com/)).

## Purpose of this open source code sensors
As world climate is changing fastly, environnement is becoming more and more un-predictable. Integrating these sensors into a low consumption embedded systems is an opportunity to measure and evaluate what's happen in your environnement, for better decision taking.

**Therefore, don't hesitate to bring your contribution to these project and broadcast widely this open-source code.**

## Why using the Raspberry Pico ?
The micro-controller is cheap, widely available, energy low comsumption and powerful (100MHz and dual core). It integrates several interfaces (UART, I2C, SPI) to match all needs for for I/O and sensors.

## Why developping in C/C++ ?
C progamming is a very well optimized langage for embedded systems : low consumption, robust, compiled, widely used...

## List of developped devices for the station

### Sensors Devices
| Sensor Name | Description | Dev Progress | Calibration |
| :---:   | :-: | :-: | :-: |
| [BME680](https://shop.pimoroni.com/products/bme680-breakout?variant=12491552129107) | Air Quality, Temperature, Pressure, Humidity Sensor | OK | ToDo |
| [VL53L1X](https://shop.pimoroni.com/products/vl53l1x-breakout?variant=12628497236051) | Time of Flight (ToF) Sensor Breakout | OK | ToDo |
| [SGP30](https://shop.pimoroni.com/products/sgp30-air-quality-sensor-breakout?variant=30924091719763) | Air Quality Sensor Breakout | OK | ToDo |
| [MAX30105](https://shop.pimoroni.com/products/max30101-breakout-heart-rate-oximeter-smoke-sensor?variant=21482065985619) | Heart Rate, Oximeter, Smoke Sensor | IN PROGRESS | ToDo |
| [PMW3901](https://shop.pimoroni.com/products/max30101-breakout-heart-rate-oximeter-smoke-sensor?variant=21482065985619) | Heart Rate, Oximeter, Smoke Sensor | ToDo | ToDo |

### IoT Devices
| Device Name | Description | Dev Progress |
| :---:   | :-: | :-: |
| [Pico Wireless](https://shop.pimoroni.com/products/bme680-breakout?variant=12491552129107) | Pico Wireless module based on ESP32 and SD Card | ToDo |

### HMI Devices
| Device Name | Description | Dev Progress | Calibration |
| :---:   | :-: | :-: | :-: |
| [ST7789](https://shop.pimoroni.com/products/bme680-breakout?variant=12491552129107) | Pico Explorer Base Display | OK | OK |

## Project Architecture
The rig used for testing is the [Pico Explorer Base](https://shop.pimoroni.com/products/pico-explorer-base?variant=32369514315859) from Pimoroni. The ST7789 display will be used to check values of sensors and buttons to select configurations of the project.

A 'Debug' compilation can be used to read sensors values from serial port (USB) on Pico.

## Compilation of the project
The code include a bash script (pico_tool.sh) to build the exe file (ELF) and load it to the Pico using OpenOCD and SWD port (using a Raspberry Pi for example). How to use the bash tool :

Display the help on the tool :
````
./pico_tool.sh -h
````
Clean-up the compilation folder (build) :
````
./pico_tool.sh -a clean
````
Generate the **makefile** using CMake. You need to deploy and set your [Raspberry Pico SDK](https://github.com/raspberrypi/pico-sdk) :
````
./pico_tool.sh -a cmake
````
build or rebuild the binary ELF executable (pico-station.elf)
````
./pico_tool.sh -a (re)build
```` 
Build the project then load the binary on RP2040 chip (see next topic for explanations) :
````
./pico_tool.sh -a build_load
```` 

## Deployment after compilation
Once the binary exe file is compiled, you can upload it using the SWD port available on the Raspberry Pico, from a GPIO module (Raspberry Pi Zero for example). 

From a Linux environnement including GPIO interface, you need to install OpenOCD following this [tutorial](https://www.electronicshub.org/programming-raspberry-pi-pico-with-swd). You have to link SWCLK/GND/SWDIO interface from Pico to a Raspberry Pi.

