/*
 *  sgp30.c file
 */

#include "sgp30.h"

/*
 *  SGP30 Commands
 */
const char SGP30_GET_MODEL_ID[2] = "\x36\x82";
const char SGP30_INIT_AIR_QUALITY[2] = "\x20\x03";
const char SGP30_MEASURE_AIR_QUALITY[2] = "\x20\x08";
const int SGP30_ADDRESS = 0x58;

/*
 *  I2C Functions
 */
static uint8_t i2c_write_byte(uint8_t address, uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    return i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

static uint8_t i2c_write_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the matrix
    return i2c_write_blocking(I2C_PORT, address, data, count, false);
}

static uint8_t i2c_write_timeout(uint8_t address, uint8_t *data, uint8_t count, uint16_t timeout) {
    // Convenience function to write a 'count' bytes to the matrix
    return i2c_write_timeout_us(I2C_PORT, address, data, count, false, timeout);
}

static uint8_t i2c_read_block(uint8_t address, uint8_t *data, uint8_t count) {
    return i2c_read_blocking(I2C_PORT, address, data, count, false);
}

/*
 *  SGP30 Sensor Functions
 */
uint32_t sgp30_init() {
    // Send the initialization command
    uint8_t buffer[2];
    uint8_t ret;

    sgp30_set_data(buffer, SGP30_INIT_AIR_QUALITY, 0);
    ret = i2c_write_timeout(SGP30_ADDRESS, buffer, sizeof(buffer), 1000);
    return ret;
}

uint8_t sgp30_get_measurement(uint8_t *reading_buffer) {
    // Get a single measurement
    uint8_t read_buffer[6];
    uint8_t cmd_buffer[2];
    uint8_t rslt = 0;

    // Request a reading
    sgp30_set_data(cmd_buffer, SGP30_MEASURE_AIR_QUALITY, 0);
    i2c_write_block(SGP30_ADDRESS, cmd_buffer, sizeof(cmd_buffer));

    // Wait a moment then read back the reading
    sleep_ms(25);
    i2c_read_block(SGP30_ADDRESS, read_buffer, sizeof(read_buffer));

    // Check the values
    uint8_t crc = read_buffer[2];
    uint16_t co2 = (read_buffer[0] << 8) | read_buffer[1];
    if (crc != sgp30_get_crc(co2)) {
        rslt = 1;
    }
    if (co2 == 0) {
        rslt = 1;
    }

    // CO2
    reading_buffer[0] = read_buffer[0];
    reading_buffer[1] = read_buffer[1];

    // TVOC
    reading_buffer[2] = read_buffer[3];
    reading_buffer[3] = read_buffer[4];

    return rslt;
}

void sgp30_set_data(uint8_t *buffer, const char *data, uint8_t start) {
    // Write the data from the command constant string into the buffer
    for (uint8_t i = start ; i < sizeof(data) ; ++i) {
        buffer[i] = data[i];
    }
}

uint8_t sgp30_get_crc(uint16_t data) {
    // Calculate an 8-bit CRC from a 16-bit word
    // See section 6.6 of the SGP30 datasheet.
    uint8_t crc = 0xFF;
    crc = sub_crc(crc, ((data & 0xFF00) >> 8));
    crc = sub_crc(crc, (data & 0x00FF));
    return crc;
}

uint8_t sub_crc(uint8_t crc, uint8_t byte) {
    crc = crc ^ byte;
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x31;
        } else {
            crc = crc << 1;
        }
    }
}

/*
 *  Misc Functions
 */
void bcd(uint16_t value, unsigned char* output_buffer) {
    // Convert the 16-bit 'value' to a Binary Coded Decimal
    // version, stored in 'output_buffer'
    // NOTE Can be refactored!
    *output_buffer = 0;
    while (value & 0xC000) {
        value -= 10000;
        *output_buffer += 1;
    }

    if (value >= 10000) {
        value -= 10000;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = 0;
    while (value & 0x3C00) {
        value -= 1000;
        *output_buffer += 1;
    }

    if (value >= 1000) {
        value -= 1000;
        *output_buffer += 1;
    }
    output_buffer++;
    *output_buffer = 0;
    while (value & 0x0780) {
        value -= 100;
        *output_buffer += 1;
    }

    if (value >= 100) {
        value -= 100;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = 0;
    while (value & 0x70) {
        value -= 10;
        *output_buffer += 1;
    }

    if (value >= 10) {
        value -= 10;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = (unsigned char)value;
}

