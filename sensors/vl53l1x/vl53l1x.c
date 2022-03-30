/*
 *  vl53l1x.c file
 */

#include "vl53l1x.h"

/*
 *  I2C Functions
 */
// Write an 8-bit register
uint8_t writeReg(uint16_t reg, uint8_t value)
{
    uint8_t buffer[3];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    buffer[2] = value;
    return i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 3, false);
}

// Write a 16-bit register
uint8_t writeReg16Bit(uint16_t reg, uint16_t value)
{
    uint8_t buffer[4];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    buffer[2] = (value >> 8) & 0xFF;
    buffer[3] = value & 0xFF;
    return i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 4, false);
}

// Write a 32-bit register
uint8_t writeReg32Bit(uint16_t reg, uint32_t value)
{
    uint8_t buffer[6];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    buffer[2] = (value >> 24) & 0xFF;
    buffer[3] = (value >> 16) & 0xFF;
    buffer[4] = (value >> 8) & 0xFF;
    buffer[5] = value & 0xFF;
    return i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 6, false);
}

// Read an 8-bit register
uint8_t readReg(uint16_t reg)
{
    uint8_t value;
    uint8_t buffer[2];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 2, true);
    i2c_read_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, &value, 1, false);
    return value;
}

// Read a 16-bit register
uint16_t readReg16Bit(uint16_t reg)
{
    uint16_t value;
    uint8_t buffer[2];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 2, true);
    i2c_read_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 2, false);
    value = buffer[0] << 8 | buffer[1];
    return value;
}

// Read a 32-bit register
uint32_t readReg32Bit(uint16_t reg)
{
    uint32_t value;
    uint8_t buffer[2];
    buffer[0] = (reg >> 8) & 0xFF;
    buffer[1] = reg & 0xFF;
    i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 2, true);
    i2c_read_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 4, false);
    value = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
    return value;
}

/*
 *  VL53L1X Sensor Functions
 */
unsigned long millis() {
    struct timeval te; 
    gettimeofday(&te, NULL); 
    unsigned long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

// Record the current time to check an upcoming timeout against
void startTimeout() { timeout_start_ms = millis(); }

uint8_t vl53l1x_init() {
    uint8_t ret = 0;
    uint32_t id;
    id = readReg16Bit(IDENTIFICATION__MODEL_ID);
    if(id != 0xEACC) {
        printf("Different with model id... (0x%x)\n", id);
        return 254;
    }

    ret = writeReg(SOFT_RESET, 0x00);
    sleep_ms(100);
    ret = writeReg(SOFT_RESET, 0x01);

    sleep_ms(100);

    startTimeout();

    writeReg(PAD_I2C_HV__EXTSUP_CONFIG, readReg(PAD_I2C_HV__EXTSUP_CONFIG) | 0x01);

    fast_osc_frequency = readReg16Bit(OSC_MEASURED__FAST_OSC__FREQUENCY);
    osc_calibrate_val = readReg16Bit(RESULT__OSC_CALIBRATE_VAL);

    writeReg16Bit(DSS_CONFIG__TARGET_TOTAL_RATE_MCPS, TargetRate); // should already be this value after reset
    writeReg(GPIO__TIO_HV_STATUS, 0x02);
    writeReg(SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS, 8); // tuning parm default
    writeReg(SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS, 16); // tuning parm default
    writeReg(ALGO__CROSSTALK_COMPENSATION_VALID_HEIGHT_MM, 0x01);
    writeReg(ALGO__RANGE_IGNORE_VALID_HEIGHT_MM, 0xFF);
    writeReg(ALGO__RANGE_MIN_CLIP, 0); // tuning parm default
    writeReg(ALGO__CONSISTENCY_CHECK__TOLERANCE, 2); // tuning parm default

    // general config
    writeReg16Bit(SYSTEM__THRESH_RATE_HIGH, 0x0000);
    writeReg16Bit(SYSTEM__THRESH_RATE_LOW, 0x0000);
    writeReg(DSS_CONFIG__APERTURE_ATTENUATION, 0x38);

    // timing config
    // most of these settings will be determined later by distance and timing
    // budget configuration
    writeReg16Bit(RANGE_CONFIG__SIGMA_THRESH, 360); // tuning parm default
    writeReg16Bit(RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS, 192); // tuning parm default

    // dynamic config
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD_0, 0x01);
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD_1, 0x01);
    writeReg(SD_CONFIG__QUANTIFIER, 2); // tuning parm default

    // VL53L1_preset_mode_standard_ranging() end

    // from VL53L1_preset_mode_timed_ranging_*
    // GPH is 0 after reset, but writing GPH0 and GPH1 above seem to set GPH to 1,
    // and things don't seem to work if we don't set GPH back to 0 (which the API
    // does here).
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD_0, 0x01);
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD_1, 0x01);
    writeReg(SD_CONFIG__QUANTIFIER, 2);
    
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD, 0x00);
    writeReg(SYSTEM__SEED_CONFIG, 1); // tuning parm default

    // from VL53L1_config_low_power_auto_mode
    writeReg(SYSTEM__SEQUENCE_CONFIG, 0x8B); // VHV, PHASECAL, DSS1, RANGE
    writeReg16Bit(DSS_CONFIG__MANUAL_EFFECTIVE_SPADS_SELECT, 200 << 8);
    writeReg(DSS_CONFIG__ROI_MODE_CONTROL, 2); // REQUESTED_EFFFECTIVE_SPADS

    setDistanceMode(Long);
    setMeasurementTimingBudget(50000);

    writeReg16Bit(ALGO__PART_TO_PART_RANGE_OFFSET_MM, readReg16Bit(MM_CONFIG__OUTER_OFFSET_MM) * 4);

    sleep_ms(100);
    startContinuous(50);

    return ret;
}

uint32_t calcMacroPeriod(uint8_t vcsel_period)
{
    uint32_t pll_period_us = ((uint32_t)0x01 << 30) / fast_osc_frequency;

    // from VL53L1_decode_vcsel_period()
    uint8_t vcsel_period_pclks = (vcsel_period + 1) << 1;

    // VL53L1_MACRO_PERIOD_VCSEL_PERIODS = 2304
    uint32_t macro_period_us = (uint32_t)2304 * pll_period_us;
    macro_period_us >>= 6;
    macro_period_us *= vcsel_period_pclks;
    macro_period_us >>= 6;

    return macro_period_us;
}

uint32_t timeoutMclksToMicroseconds(uint32_t timeout_mclks, uint32_t macro_period_us)
{
    return ((uint64_t)timeout_mclks * macro_period_us + 0x800) >> 12;
}

uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_us, uint32_t macro_period_us)
{
    return (((uint32_t)timeout_us << 12) + (macro_period_us >> 1)) / macro_period_us;
}

uint32_t decodeTimeout(uint16_t reg_val)
{
    return ((uint32_t)(reg_val & 0xFF) << (reg_val >> 8)) + 1;
}

uint16_t encodeTimeout(uint32_t timeout_mclks)
{
    // encoded format: "(LSByte * 2^MSByte) + 1"
    uint32_t ls_byte = 0;
    uint16_t ms_byte = 0;

    if (timeout_mclks > 0)
    {
        ls_byte = timeout_mclks - 1;

        while ((ls_byte & 0xFFFFFF00) > 0)
        {
        ls_byte >>= 1;
        ms_byte++;
        }

        return (ms_byte << 8) | (ls_byte & 0xFF);
    }
    else { return 0; }
}

float countRateFixedToFloat(uint16_t count_rate_fixed) { return (float)count_rate_fixed / (1 << 7); }

uint32_t getMeasurementTimingBudget()
{
    uint32_t macro_period_us = calcMacroPeriod(readReg(RANGE_CONFIG__VCSEL_PERIOD_A));

    uint32_t range_config_timeout_us = timeoutMclksToMicroseconds(decodeTimeout(
    readReg16Bit(RANGE_CONFIG__TIMEOUT_MACROP_A)), macro_period_us);

    return  2 * range_config_timeout_us + 4528;
}

uint8_t setMeasurementTimingBudget(uint32_t budget_us)
{
    // assumes PresetMode is LOWPOWER_AUTONOMOUS
    if (budget_us <= 4528) { return 1; }

    uint32_t range_config_timeout_us = budget_us -= 4528;
    if (range_config_timeout_us > 1100000) { return 1; } // FDA_MAX_TIMING_BUDGET_US * 2

    range_config_timeout_us /= 2;

    uint32_t macro_period_us;

    macro_period_us = calcMacroPeriod(readReg(RANGE_CONFIG__VCSEL_PERIOD_A));

    uint32_t phasecal_timeout_mclks = timeoutMicrosecondsToMclks(1000, macro_period_us);
    if (phasecal_timeout_mclks > 0xFF) { phasecal_timeout_mclks = 0xFF; }
    writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, phasecal_timeout_mclks);

    writeReg16Bit(MM_CONFIG__TIMEOUT_MACROP_A, encodeTimeout(
        timeoutMicrosecondsToMclks(1, macro_period_us)));

    writeReg16Bit(RANGE_CONFIG__TIMEOUT_MACROP_A, encodeTimeout(
        timeoutMicrosecondsToMclks(range_config_timeout_us, macro_period_us)));

    macro_period_us = calcMacroPeriod(readReg(RANGE_CONFIG__VCSEL_PERIOD_B));

    writeReg16Bit(MM_CONFIG__TIMEOUT_MACROP_B, encodeTimeout(
        timeoutMicrosecondsToMclks(1, macro_period_us)));

    writeReg16Bit(RANGE_CONFIG__TIMEOUT_MACROP_B, encodeTimeout(
        timeoutMicrosecondsToMclks(range_config_timeout_us, macro_period_us)));

    return 1;
}


uint8_t setDistanceMode(enum DistanceMode mode)
{
    uint32_t budget_us = getMeasurementTimingBudget();

    switch (mode)
    {
        case Short:
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
          writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);

          writeReg(SD_CONFIG__WOI_SD0, 0x07);
          writeReg(SD_CONFIG__WOI_SD1, 0x05);
          writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 6); 
          writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 6); 

          break;

        case Medium:
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0B);
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x09);
          writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0x78);

          writeReg(SD_CONFIG__WOI_SD0, 0x0B);
          writeReg(SD_CONFIG__WOI_SD1, 0x09);
          writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 10); 
          writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 10); 

          break;

        case Long:
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
          writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
          writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);

          writeReg(SD_CONFIG__WOI_SD0, 0x0F);
          writeReg(SD_CONFIG__WOI_SD1, 0x0D);
          writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 14); 
          writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 14); 

          break;

        default:
          return 0;
    }
}

void startContinuous(uint32_t period_ms)
{
    // from VL53L1_set_inter_measurement_period_ms()
    writeReg32Bit(SYSTEM__INTERMEASUREMENT_PERIOD, period_ms * osc_calibrate_val);
    writeReg(SYSTEM__INTERRUPT_CLEAR, 0x01); // sys_interrupt_clear_range
    writeReg(SYSTEM__MODE_START, 0x40); // mode_range__timed
}

uint16_t vl53l1x_read_results(ResultBuffer *results)
{
    uint8_t buffer[2];
    uint8_t data[17];
    buffer[0] = (RESULT__RANGE_STATUS >> 8) & 0xFF;
    buffer[1] = RESULT__RANGE_STATUS & 0xFF;
    i2c_write_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, buffer, 2, true);
    i2c_read_blocking(I2C_VL53L1X_PORT, VL53L1X_ADDRESS, data, 17, false);

    //ResultBuffer results;

    results->range_status = data[0];
    // bus->read(); // report_status: not used
    results->stream_count = data[2];

    results->dss_actual_effective_spads_sd0  = (uint16_t)data[3] << 8; // high byte
    results->dss_actual_effective_spads_sd0 |=           data[4];      // low byte

    // bus->read(); // peak_signal_count_rate_mcps_sd0: not used
    // bus->read();

    results->ambient_count_rate_mcps_sd0  = (uint16_t)data[7] << 8; // high byte
    results->ambient_count_rate_mcps_sd0 |=           data[8];      // low byte

    // bus->read(); // sigma_sd0: not used
    // bus->read();

    // bus->read(); // phase_sd0: not used
    // bus->read();

    results->final_crosstalk_corrected_range_mm_sd0  = (uint16_t)data[13] << 8; // high byte
    results->final_crosstalk_corrected_range_mm_sd0 |=           data[14];      // low byte

    results->peak_signal_count_rate_crosstalk_corrected_mcps_sd0  = (uint16_t)data[15] << 8; // high byte
    results->peak_signal_count_rate_crosstalk_corrected_mcps_sd0 |=           data[16];      // low byte

    //return results->final_crosstalk_corrected_range_mm_sd0;
    return 0;
}
