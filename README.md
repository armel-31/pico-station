# pico-station

The aim of this project is to propose open code in C for the [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html), to integrate several environnemental sensors (Pimoroni breakouts) :

| Sensor Name | Description | Dev Progress | Calibration |
| :---:   | :-: | :-: | :-: |
| [ST7789](https://shop.pimoroni.com/products/bme680-breakout?variant=12491552129107) | Pico Explorer Base Display | OK | OK |
| [BME680](https://shop.pimoroni.com/products/bme680-breakout?variant=12491552129107) | Air Quality, Temperature, Pressure, Humidity Sensor | OK | ToDo |
| [VL53L1X](https://shop.pimoroni.com/products/vl53l1x-breakout?variant=12628497236051) | Time of Flight (ToF) Sensor Breakout | OK | ToDo |
| [SGP30](https://shop.pimoroni.com/products/sgp30-air-quality-sensor-breakout?variant=30924091719763) | Air Quality Sensor Breakout | OK | ToDo |
| [MAX30105](https://shop.pimoroni.com/products/max30101-breakout-heart-rate-oximeter-smoke-sensor?variant=21482065985619) | Heart Rate, Oximeter, Smoke Sensor | IN PROGRESS | ToDo |

The rig used for testing is the [Pico Explorer Base](https://shop.pimoroni.com/products/pico-explorer-base?variant=32369514315859) from Pimoroni. The ST7789 display will be used to check values of sensors.

The code include a bash script (pico_tool.sh) to build the exe file and load it to the Pico using OpenOCD and SWD port (using a Raspberry Pi for example).
