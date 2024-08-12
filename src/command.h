#ifndef __COMMAND_H
#define __COMMAND_H

#include <hardware/flash.h>

#include "tusb.h"

#define PROVIDER_FIRE 0x00
#define COMMAND_FIRE_ON 0x00
#define COMMAND_FIRE_OFF 0x01

#define PROVIDER_ADC0 0x01
#define COMMAND_ADC0_READ 0x00
#define COMMAND_ADC0_TRGT 0x01
#define COMMAND_ADC0_SET 0x02

// #define PROVIDER_ADC1 0x02
// #define COMMAND_ADC1_READ 0x00
// #define COMMAND_ADC1_TRGT 0x01
// #define COMMAND_ADC1_SET 0x02

#define PROVIDER_PIN 0x03
#define COMMAND_PIN_READ0 0x00
#define COMMAND_PIN_READ1 0x00

#define PROVIDER_MOTOR1 0x04
#define COMMAND_MOTOR1_OPEN 0x00
#define COMMAND_MOTOR1_CLOSE 0x01
#define COMMAND_MOTOR1_STATE 0x02
#define COMMAND_MOTOR1_INITS 0x04

#define PROVIDER_MOTOR2 0x05
#define COMMAND_MOTOR2_OPEN 0x00
#define COMMAND_MOTOR2_CLOSE 0x01
#define COMMAND_MOTOR2_STATE 0x02
#define COMMAND_MOTOR2_INITS 0x03

#define PROVIDER_STATE 0x06
#define COMMAND_STATE_READ 0x00

#define STATE_ACCEL_X 0x0000
#define STATE_ACCEL_Y 0x0001
#define STATE_ACCEL_Z 0x0002
#define STATE_GYRO_X 0x0003
#define STATE_GYRO_Y 0x0004
#define STATE_GYRO_Z 0x0005
#define STATE_TEMP 0x0006

#define PROVIDER_CONFIG 0x07
#define COMMAND_CONFIG_READ 0x00
#define COMMAND_CONFIG_SET 0x01

#define CONFIG_NO_USE_ADC0 0x0000
#define CONFIG_USE_ADC0 0x0001
#define CONFIG_NO_USE_ADC1 0x0100
#define CONFIG_USE_ADC1 0x0101
#define CONFIG_NO_USE_PIN0 0x0200
#define CONFIG_USE_PIN0 0x0201
#define CONFIG_NO_USE_PIN1 0x0300
#define CONFIG_USE_PIN1 0x0301

typedef struct
{
	uint8_t buf[10];
	uint16_t buf_index;
	uint8_t data[4];
	uint8_t crc;

	uint32_t motor1_wrap;
	uint32_t motor2_wrap;
	bool motor1_state = false;
	bool motor2_state = false;
	uint8_t flash_data[FLASH_PAGE_SIZE];
} state_t;

void state_init(state_t &state);
void process_command(state_t &state);

#endif