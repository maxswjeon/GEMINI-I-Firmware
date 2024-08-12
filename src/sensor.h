#ifndef __SENSOR_H
#define __SENSOR_H

#include "defs.h"

#include <string.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include <hardware/i2c.h>

#define POS_OPEN(wrap) (2.72 / 100.0 * (wrap))
#define POS_CLOSE(wrap) (4.27 / 100.0 * (wrap))

extern uint64_t imu_last_update;
extern int16_t imu_accel[3];
extern int16_t imu_gyro[3];
extern int16_t temp;
extern uint16_t adc0_value;

void sensor_main();

void rtc_setup();
#if RTC_CONFIGURE_SUPPORT
void rtc_configure();
#endif

// IMU (BHI260AP)
void imu_setup();
void imu_inst_setup();
bool imu_reset();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
#endif // __SENSOR_H