#ifndef __SENSOR_H
#define __SENSOR_H

#include <string.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include <hardware/spi.h>

#define WAIT_TRUE 0
#define WAIT_FALSE 1
#define WAIT_NONE 2

void rtc_setup();
void sensor_main();

// IMU (BHI260AP)
void imu_setup();
void imu_inst_setup();
uint8_t imu_read_reg(uint8_t address);
void imu_write_reg(uint8_t address, uint8_t value);
void imu_write_data(uint8_t address, const uint8_t *data, size_t len);
bool imu_reset();
void imu_reset_hard();
void imu_reset_soft();

#endif // __SENSOR_H