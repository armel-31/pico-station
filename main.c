/*
 *    Pico-Station
 *
 *    A. Surzur - 2022
 *    main.c file
 */

#include "pico-station.h"

#include <sgp30.h>
#include <vl53l1x.h>
#include <st7789.h>
#include <bme680.h>

/*
 * Init GPIO
 */
void init_gpio() {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // SPI Initialization
    spi_init(SPI_PORT, 16000000);

    gpio_init(DC_PIN);
    gpio_init(CS_PIN);
    gpio_init(BK_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_set_dir(BK_PIN, GPIO_OUT);

    // Set SPI format spi, bits, polarity (CPOL), phase (CPHA)
    spi_set_format(SPI_PORT, 8, 1, 0, 1);

    // SPI Pins Initialization
    gpio_set_function(DC_PIN, GPIO_FUNC_SIO);
    gpio_set_function(CS_PIN, GPIO_FUNC_SIO);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);

    // I2C Initialization
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // Buttons Initialization
    gpio_set_function(SWITCH_A, GPIO_FUNC_SIO);
    gpio_set_function(SWITCH_B, GPIO_FUNC_SIO);
    gpio_set_function(SWITCH_X, GPIO_FUNC_SIO);
    gpio_set_function(SWITCH_Y, GPIO_FUNC_SIO);
    gpio_set_dir(SWITCH_A, GPIO_IN);
    gpio_set_dir(SWITCH_B, GPIO_IN);
    gpio_set_dir(SWITCH_X, GPIO_IN);
    gpio_set_dir(SWITCH_Y, GPIO_IN);
    gpio_pull_down(SWITCH_A);
}

/*
 * Init BME680
 */


/*
 * Runtime main loop
 */
int main() {

    uint8_t ui8_run_loop = 1;
    // ST7789 display values
    uint8_t disp_y;
    char disp_data[DISP_LEN];

    // SGP30 reading values
    uint32_t sgp30_dev_init = 0;
    uint8_t reading_buffer[4];
    unsigned char decimal_reading[5];
    uint16_t co2_reading = 0;
    uint16_t tvoc_reading = 0;

    // BME680 reading values
    uint8_t bme680_dev_init = BME680_OK;
    struct bme680_dev t_dev;
    uint8_t set_required_settings;
    struct bme680_field_data data;
    uint16_t meas_period;

    // VL53L1X reading values
    uint8_t vl53l1x_dev_init = 0;
    ResultBuffer t_results;
    uint16_t tof_value = 0;

    // Pico SDK initialization
    stdio_init_all();
    init_gpio();

    ///// ST7789 Initialization
    display_init();

    ///// SGP30 Initialization
    sgp30_dev_init = sgp30_init();

    ///// VL53L1X Initialization
    vl53l1x_dev_init = vl53l1x_init();

    ///// BME680 Initialization
    /* Copy required parameters from reference bme680_dev struct */
    t_dev.dev_id = BME680_I2C_ADDR_PRIMARY;
    t_dev.amb_temp = 20;
    t_dev.read = (bme680_com_fptr_t)&user_i2c_read;
    t_dev.write = (bme680_com_fptr_t)&user_i2c_write;
    t_dev.intf = BME680_I2C_INTF;
    t_dev.delay_ms = (bme680_delay_fptr_t)&user_delay_ms;
    bme680_dev_init = bme680_init(&t_dev);

    /* Set the temperature, pressure and humidity settings */
    t_dev.tph_sett.os_hum = BME680_OS_16X;
    t_dev.tph_sett.os_pres = BME680_OS_1X;
    t_dev.tph_sett.os_temp = BME680_OS_2X;
    t_dev.tph_sett.filter = BME680_FILTER_SIZE_0;

    /* Set the remaining gas sensor settings and link the heating profile */
    t_dev.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    t_dev.gas_sett.heatr_temp = 300; /* degree Celsius */
    t_dev.gas_sett.heatr_dur = 100; /* milliseconds */

    /* Select the power mode */
    t_dev.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    bme680_set_sensor_settings(set_required_settings,&t_dev);

    /* Set the power mode */
    bme680_set_sensor_mode(&t_dev);
    bme680_get_profile_dur(&meas_period, &t_dev);

    // Loop
    while (ui8_run_loop) {
        // Set LED to 1
        gpio_put(LED_PIN, 1);
        user_delay_ms(meas_period); /* Delay till the measurement is ready */
        
        disp_y = 0;
        memset(disp_data, 0, DISP_LEN * sizeof(char));

        if (sgp30_dev_init != 254 && sgp30_get_measurement(reading_buffer) == 0) {
            co2_reading = (reading_buffer[0] << 8) | reading_buffer[1];
            tvoc_reading = (reading_buffer[3] << 8) | reading_buffer[4];

            //SGP30 displayed data
            if (disp_y != 0) {
                disp_y = disp_y + 20;
            }
            sprintf(disp_data, "CO2  : %u ppm", co2_reading);
            writeString(0, disp_y, disp_data, Font_11x18, RED, BLACK);
            disp_y = disp_y + 20;
            sprintf(disp_data, "TVOC : %u ppb", tvoc_reading);
            writeString(0, disp_y, disp_data, Font_11x18, RED, BLACK);
#ifdef DEBUG
            printf("CO2 : %u - TVOC : %u\n", co2_reading, tvoc_reading);
#endif
        }

        if (bme680_get_sensor_data(&data, &t_dev) == BME680_OK && bme680_dev_init == BME680_OK) {
            // BME680 displayed data
            if (disp_y != 0) {
                disp_y = disp_y + 20;
            }
            sprintf(disp_data, "Temp : %.2f degC", data.temperature);
            writeString(0, disp_y, disp_data, Font_11x18, GREEN, BLACK);
            disp_y = disp_y + 20;
            sprintf(disp_data, "Press: %.2f hPa", data.pressure / 100.0f);
            writeString(0, disp_y, disp_data, Font_11x18, GREEN, BLACK);
            disp_y = disp_y + 20;
            sprintf(disp_data, "Humid: %.2f H%%", data.humidity);
            writeString(0, disp_y, disp_data, Font_11x18, GREEN, BLACK);
            disp_y = disp_y + 20;
            sprintf(disp_data, "GasR : %.2f KOhms", data.gas_resistance);
            writeString(0, disp_y, disp_data, Font_11x18, GREEN, BLACK);
//#ifdef DEBUG
            printf("T: %.2f degC, P: %.2f hPa, H %.2f H\n", data.temperature, data.pressure / 100.0f, data.humidity);
//#endif
            /* Trigger the next measurement if you would like to read data out continuously */
            if (t_dev.power_mode == BME680_FORCED_MODE) {
                bme680_set_sensor_mode(&t_dev);
            }
        }

        if (vl53l1x_dev_init != 254 && vl53l1x_read_results(&t_results) == 0) {
            tof_value = t_results.final_crosstalk_corrected_range_mm_sd0;

            //VL53L1X displayed data
            if (disp_y != 0) {
                disp_y = disp_y + 20;
            }
            sprintf(disp_data, "TOF  : %u mm", tof_value);
            writeString(0, disp_y, disp_data, Font_11x18, BLUE, BLACK);
#ifdef DEBUG
            printf("TOF: %d mm\n", tof_value);
#endif
        }

        if(gpio_get(SWITCH_A)) {
            sprintf(disp_data, "Button A pressed");
            writeString(0, disp_y + 20, disp_data, Font_11x18, BLUE,BLACK);
        }

        // Reset LED value
        gpio_put(LED_PIN, 0);
        
        // Timer between 2 readings
        sleep_ms(500);

    }

    return 0;
}
