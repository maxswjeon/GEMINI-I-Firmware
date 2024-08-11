#ifndef __SENSOR_H
#define __SENSOR_H

#include "defs.h"

#include <string.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include <hardware/spi.h>

#define WAIT_TRUE 0
#define WAIT_FALSE 1
#define WAIT_NONE 2

#define FIFO_WAKEUP 1
#define FIFO_NON_WAKEUP 2
#define FIFO_STATUS 3

#define DATA_NONE 0
#define DATA_QUAT 1
#define DATA_EULR 2
#define DATA_VEC3 3
#define DATA_UINT8 4
#define DATA_UINT16 5
#define DATA_UINT24 6
#define DATA_UINT32 7
#define DATA_EVT_NONE 8
#define DATA_EVT_TAP 9
#define DATA_EVT_ACT 10
#define DATA_EVT_WST 11
#define DATA_AIRQ 12
#define DATA_META 13
#define DATA_DEBG 14
#define DATA_TIME 15

#define COMMAND_UNDEFINED(len, target_len, type)                           \
	if (len < target_len)                                                  \
		return -4;                                                         \
	tud_cdc_printf("[WARN] %s command type is not implemented\r\n", type); \
	return target_len;

extern uint8_t fifo_command_table0[];
extern uint8_t fifo_command_table1[];
extern uint8_t fifo_command_table3[];
extern uint8_t fifo_command_table4[];
extern uint8_t fifo_command_table7[];

void sensor_main();

void rtc_setup();
#if RTC_CONFIGURE_SUPPORT
void rtc_configure();
#endif

// IMU (BHI260AP)
void imu_setup();
void imu_inst_setup();
void imu_sensor_start(uint8_t sensor_id, float rate);
uint8_t imu_read_reg(uint8_t address);
void imu_write_reg(uint8_t address, uint8_t value);
void imu_write_data(uint8_t address, const uint8_t *data, size_t len);
bool imu_reset();
void imu_reset_hard();
void imu_reset_soft();
uint8_t imu_update();
int32_t imu_fifo_task(uint8_t fifo_index);
int32_t imu_command(uint8_t fifo_index, uint8_t *buffer, uint16_t len);

void imu_process_quaternion(uint8_t *buffer, uint16_t len);
void imu_process_euler(uint8_t *buffer, uint16_t len);
void imu_process_vec3(uint8_t *buffer, uint16_t len);
void imu_process_uint16(uint8_t *buffer, uint16_t len);
void imu_process_uint24(uint8_t *buffer, uint16_t len);
uint8_t imu_process_time(uint8_t *buffer, uint16_t len);
void imu_process_meta(uint8_t *buffer, uint16_t len);

#endif // __SENSOR_H