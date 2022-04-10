/*
 *  sgp30.h file
 */

#ifndef _SGP30_HEADER_
#define _SGP30_HEADER_

/*
 *  Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#include "pico-station.h"

/*
 *  Prototypes
 */
static uint8_t i2c_write_byte(uint8_t address, uint8_t byte);
static uint8_t i2c_write_block(uint8_t address, uint8_t* data, uint8_t count);
static uint8_t i2c_write_timeout(uint8_t address, uint8_t* data, uint8_t count, uint16_t timeout);
static uint8_t i2c_read_block(uint8_t address, uint8_t *data, uint8_t count);

uint32_t sgp30_init();
uint8_t sgp30_get_measurement(uint8_t *reading_buffer);
void sgp30_set_data(uint8_t *buffer, const char *data, uint8_t start);
uint8_t sgp30_get_crc(uint16_t data);
uint8_t sub_crc(uint8_t crc, uint8_t byte);

void bcd(uint16_t value, unsigned char* output_buffer);

#endif  // _SGP30_HEADER_
