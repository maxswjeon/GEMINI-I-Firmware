#include "util.h"
#include "flash.h"
#include "sensor.h"
#include "command.h"

#include <pico/stdlib.h>

#include <hardware/adc.h>
#include <hardware/pwm.h>
#include <hardware/flash.h>
#include <hardware/watchdog.h>

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

void state_init(state_t &state)
{
	for (size_t i = 0; i < FLASH_PAGE_SIZE; ++i)
	{
		state.flash_data[i] = flash_target_contents[i];
	}

	for (size_t i = 0; i < 10; ++i)
	{
		state.buf[i] = 0;
	}

	state.buf_index = 0;

	for (size_t i = 0; i < 4; ++i)
	{
		state.data[i] = 0;
	}

	state.crc = 0;

	state.motor1_wrap = init_pwm(16);
	state.motor2_wrap = init_pwm(17);

	state.motor1_state = state.flash_data[FLASH_MOTOR1_INITS] == 1;
	state.motor2_state = state.flash_data[FLASH_MOTOR2_INITS] == 1;
}

void process_command(state_t &state)
{
	uint8_t c = uart_getc(uart0);
	if (c == '\r')
	{
		return;
	}

	if (state.buf_index != 9)
	{
		state.buf[state.buf_index++] = c;

		for (int i = 0; i < state.buf_index; ++i)
		{
			tud_cdc_write_char(state.buf[i]);
		}
		tud_cdc_write_char('\n');
		return;
	}

	for (int i = 0; i < 9; ++i)
	{
		state.buf[i] = state.buf[i + 1];
	}
	state.buf[9] = c;

	for (int i = 0; i <= state.buf_index; ++i)
	{
		tud_cdc_write_char(state.buf[i]);
	}
	tud_cdc_write_char('\n');

	if (state.buf[0] != '$' || state.buf[9] != '#')
	{
		return;
	}
	state.buf_index = 0;

	tud_cdc_write_char(uint16_to_hstr(state.data[0], 0x04));
	tud_cdc_write_char(uint16_to_hstr(state.data[0], 0x00));
	tud_cdc_write_char(uint16_to_hstr(state.data[1], 0x04));
	tud_cdc_write_char(uint16_to_hstr(state.data[1], 0x00));
	tud_cdc_write_char(uint16_to_hstr(state.data[2], 0x04));
	tud_cdc_write_char(uint16_to_hstr(state.data[2], 0x00));
	tud_cdc_write_char(uint16_to_hstr(state.data[3], 0x04));
	tud_cdc_write_char(uint16_to_hstr(state.data[3], 0x00));
	tud_cdc_write_char('\n');
	int16_t res = hstr_to_uint16(state.buf + 1);
	if (res < 0)
	{
		uart_puts(uart0, "$00010000#"); // ACK - Invalid packet (data 0)
		return;
	}
	state.data[0] = (uint8_t)res;

	res = hstr_to_uint16(state.buf + 3);
	if (res < 0)
	{
		uart_puts(uart0, "$00010001#"); // ACK - Invalid packet (data 1)
		return;
	}
	state.data[1] = (uint8_t)res;

	res = hstr_to_uint16(state.buf + 5);
	if (res < 0)
	{
		uart_puts(uart0, "$00010002#"); // ACK - Invalid packet (data 2)
		return;
	}
	state.data[2] = (uint8_t)res;

	res = hstr_to_uint16(state.buf + 7);
	if (res < 0)
	{
		uart_puts(uart0, "$00010003#"); // ACK - Invalid packet (data 3)
		return;
	}
	state.data[3] = (uint8_t)res;

	// res = hstr_to_uint16(state.buf + 9);
	// if (res < 0)
	// {
	// 	uart_puts(uart0, "$0001FFFF#"); // ACK - Invalid packet (crc)
	// 	return;
	// }
	// state.crc = (uint8_t)res;

	// if (state.crc != gencrc(state.data, 4))
	// {
	// 	uart_puts(uart0, "$00020000#"); // ACK - CRC mismatch
	// 	return;
	// }

	if (state.data[0] == PROVIDER_FIRE)
	{
		if (state.data[1] == COMMAND_FIRE_ON)
		{
			gpio_put(13, 1);
			uart_puts(uart0, "$00000000#"); // ACK - Success
			return;
		}

		if (state.data[1] == COMMAND_FIRE_OFF)
		{
			gpio_put(13, 0);
			uart_puts(uart0, "$00010000#"); // ACK - Success
			return;
		}
	}

	if (state.data[0] == PROVIDER_ADC0)
	{
		if (state.data[1] == COMMAND_ADC0_READ)
		{
			uart_puts(uart0, "$0100"); // ACK - Success
			uart_putc(uart0, uint16_to_hstr(adc0_value, 0x0C));
			uart_putc(uart0, uint16_to_hstr(adc0_value, 0x08));
			uart_putc(uart0, uint16_to_hstr(adc0_value, 0x04));
			uart_putc(uart0, uint16_to_hstr(adc0_value, 0x00));
			uart_putc(uart0, '#');
			return;
		}

		if (state.data[1] == COMMAND_ADC0_TRGT)
		{
			uart_puts(uart0, "$0101"); // ACK - Success
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[0], 0x04));
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[0], 0x00));
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[1], 0x04));
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[1], 0x00));
			uart_putc(uart0, '#');
			return;
		}

		if (state.data[1] == COMMAND_ADC0_SET)
		{
			state.flash_data[0] = state.data[2];
			state.flash_data[1] = state.data[3];
			flash_range_program(FLASH_TARGET_OFFSET, state.flash_data, FLASH_PAGE_SIZE);
			uart_puts(uart0, "$0202");
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[0], 0x04));
			uart_putc(uart0, uint16_to_hstr(flash_target_contents[1], 0x00));
			uart_putc(uart0, '#');
			return;
		}
	}

	if (state.data[0] == PROVIDER_PIN)
	{
		if (state.data[1] == COMMAND_PIN_READ0)
		{
			uart_puts(uart0, "$0300000"); // ACK - Success
			uart_putc(uart0, gpio_get(22) ? '1' : '0');
			uart_putc(uart0, '#');
			return;
		}

		if (state.data[1] == COMMAND_PIN_READ1)
		{
			uart_puts(uart0, "$0300000"); // ACK - Success
			uart_putc(uart0, gpio_get(27) ? '1' : '0');
			uart_putc(uart0, '#');
			return;
		}
	}

	if (state.data[0] == PROVIDER_MOTOR1)
	{
		if (state.data[1] == COMMAND_MOTOR1_OPEN)
		{
			pwm_set_chan_level(17, 0, POS_OPEN(state.motor1_wrap));
			uart_puts(uart0, "$04000000#"); // ACK - Success
			return;
		}

		if (state.data[1] == COMMAND_MOTOR1_CLOSE)
		{
			pwm_set_chan_level(17, 0, POS_OPEN(state.motor1_wrap));
			uart_puts(uart0, "$04010000#"); // ACK - Success
			return;
		}

		if (state.data[1] == COMMAND_MOTOR1_STATE)
		{
			uart_puts(uart0, "$0402000"); // ACK - Success
			uart_putc(uart0, state.motor1_state ? '1' : '0');
			uart_putc(uart0, '#');
			return;
		}

		if (state.data[1] == COMMAND_MOTOR1_INITS)
		{
			state.motor1_state = !(state.data[2] != 0);
			state.flash_data[FLASH_MOTOR1_INITS] = state.motor1_state ? 1 : 0;
			flash_range_program(FLASH_TARGET_OFFSET, state.flash_data, FLASH_PAGE_SIZE);
			uart_puts(uart0, "$04030000#"); // ACK - Success
			return;
		}
	}

	if (state.data[0] == PROVIDER_MOTOR2)
	{
		if (state.data[1] == COMMAND_MOTOR2_OPEN)
		{
			pwm_set_chan_level(17, 0, POS_OPEN(state.motor2_wrap));
			uart_puts(uart0, "$05000000#"); // ACK - Success
			return;
		}

		if (state.data[1] == COMMAND_MOTOR2_CLOSE)
		{
			pwm_set_chan_level(17, 0, POS_CLOSE(state.motor2_wrap));
			uart_puts(uart0, "$05010000#"); // ACK - Success
			return;
		}

		if (state.data[1] == COMMAND_MOTOR2_STATE)
		{
			uart_puts(uart0, "$0502000"); // ACK - Success
			uart_putc(uart0, state.motor2_state ? '1' : '0');
			uart_putc(uart0, '#');
			return;
		}

		if (state.data[1] == COMMAND_MOTOR2_INITS)
		{
			state.motor2_state = !(state.data[2] != 0);
			state.flash_data[FLASH_MOTOR2_INITS] = state.motor2_state ? 1 : 0;
			flash_range_program(FLASH_TARGET_OFFSET, state.flash_data, FLASH_PAGE_SIZE);
			uart_puts(uart0, "$05030000#"); // ACK - Success
			return;
		}
	}

	if (state.data[0] == PROVIDER_STATE)
	{
		if (state.data[1] == COMMAND_STATE_READ)
		{
			if (state.data[3] == STATE_ACCEL_X & 0xFF)
			{
				uart_puts(uart0, "$0600"); // ACK - Success
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x0C));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x08));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x04));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x00));
				uart_putc(uart0, '#');
				return;
			}

			if (state.data[3] == STATE_ACCEL_Y & 0xFF)
			{
				uart_puts(uart0, "$0600"); // ACK - Success
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x0C));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x08));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x04));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x00));
				uart_putc(uart0, '#');
				return;
			}

			if (state.data[3] == STATE_ACCEL_Z & 0xFF)
			{
				uart_puts(uart0, "$0600"); // ACK - Success
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x0C));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x08));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x04));
				uart_putc(uart0, uint16_to_hstr(imu_accel[0], 0x00));
				uart_putc(uart0, '#');
				return;
			}
		}
	}

	tud_cdc_write_char(uint16_to_hstr(state.data[0], 0x04));
	tud_cdc_write_char(uint16_to_hstr(state.data[0], 0x00));
	tud_cdc_write_char('\n');
	if (state.data[0] == 0xFF)
	{
		uart_puts(uart0, "$FFFF0000#"); // ACK - Ping
		return;
	}
}