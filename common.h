/*
 *  common.h file
 */

#ifndef _MAIN_HEADER
#define _MAIN_HEADER

/*
 *  Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

/*
 *  Define variables
 */

#define DEBUG
#define LED_PIN         25
#define SWITCH_A        12
#define SWITCH_B        13
#define SWITCH_X        14
#define SWITCH_Y        15
#define DISP_LEN        256

/* I2C definition */
#define I2C_PORT	    i2c0
#define I2C_FREQUENCY	400000
#define SDA_GPIO	    20
#define SCL_GPIO	    21

/* SPI definition */
#define SPI_PORT	spi0

#define SCK_PIN 	18 //SClock
#define MOSI_PIN    19 //SData
#define DC_PIN	    16  //Data/Command
#define CS_PIN	    17 // Chip Select
#define BK_PIN	    22 //Backlight

/*
 * Prototype
 */


#endif  //  _MAIN_HEADER
