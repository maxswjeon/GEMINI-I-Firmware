#include "defs.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <pico/platform.h>
#include <pico/multicore.h>
#include <pico/runtime.h>
#include <pico/sync.h>

#include <hardware/pwm.h>
#include <hardware/clocks.h>

#include "tusb.h"

// auto_init_mutex(serial_mutex);

bool core1_panic = false;
char core1_panic_message[2048] = {
	0,
};

int32_t tud_cdc_buffer[SERIAL_BUFFER_SIZE];
uint32_t tud_cdc_buffer_head = 0;
uint32_t tud_cdc_buffer_tail = 0;

void tud_cdc_vprintf(const char *fmt, va_list args)
{
	char message[2048];
	memset(message, 0, sizeof(message));

	size_t len = vsnprintf(NULL, 0, fmt, args) + 1;

	va_list args2;
	va_copy(args2, args);
	vsprintf(message, fmt, args2);
	va_end(args2);

	tud_cdc_write_str(message);
}

void __printflike(1, 0) tud_cdc_printf(const char *fmt, ...)
{
	// mutex_enter_blocking(&serial_mutex);

	va_list args;
	va_start(args, fmt);
	tud_cdc_vprintf(fmt, args);
	va_end(args);

	// mutex_exit(&serial_mutex);
}

void __attribute__((noreturn)) __printflike(1, 0) tusb_panic(const char *fmt, ...)
{
	if (get_core_num() == 1)
	{
		va_list args1, args2;
		va_start(args1, fmt);

		size_t len = vsnprintf(NULL, 0, fmt, args1) + 1;
		va_end(args1);

		memset(core1_panic_message, 0, sizeof(core1_panic_message));
		vsprintf(core1_panic_message, fmt, args2);
		va_end(args2);

		core1_panic = true;

		multicore_reset_core1();
		while (true)
		{
			tight_loop_contents();
		}
	}

	if (core1_panic)
	{
		tud_cdc_write_str("\r\n\r\npanic: core1 panic\r\n");

		if (core1_panic_message[0])
		{
			tud_cdc_write_str(core1_panic_message);
		}

		while (true)
		{
			tight_loop_contents();
		}
	}
	else
	{
		tud_cdc_write_str("\r\n\r\npanic: core0 panic\r\n");
		va_list args;
		va_start(args, fmt);
		tud_cdc_vprintf(fmt, args);

		while (true)
		{
			tud_cdc_write_flush();
			tud_task();
			tuh_task();
		}
	}
}

void tud_cdc_buffer_init()
{
	memset(tud_cdc_buffer, 0, sizeof(tud_cdc_buffer));
}

int32_t tud_cdc_buffer_read()
{
	if (tud_cdc_buffer_head == tud_cdc_buffer_tail)
	{
		return -1;
	}

	int32_t c = tud_cdc_buffer[tud_cdc_buffer_head];
	tud_cdc_buffer_head = (tud_cdc_buffer_head + 1) % SERIAL_BUFFER_SIZE;
	return c;
}

void tud_cdc_buffer_write(int32_t c)
{
	if ((tud_cdc_buffer_tail + 1) % SERIAL_BUFFER_SIZE == tud_cdc_buffer_head)
	{
		return;
	}
	tud_cdc_buffer[tud_cdc_buffer_tail] = c;
	tud_cdc_buffer_tail = (tud_cdc_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
}

void tud_cdc_buffer_flush()
{
	tud_cdc_buffer_head = 0;
	tud_cdc_buffer_tail = 0;
}

int8_t tud_cdc_buffer_get_int8(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int8_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

int16_t tud_cdc_buffer_get_int16(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int16_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

int32_t tud_cdc_buffer_get_int32(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int32_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

uint8_t tud_cdc_buffer_get_uint8(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint8_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

uint16_t tud_cdc_buffer_get_uint16(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint16_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

uint32_t tud_cdc_buffer_get_uint32(bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint32_t value = 0;
	while (true)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			continue;
		}

		if (c == '\n' || c == '\r')
		{
			break;
		}
	}

	return value;
}

int8_t tud_cdc_buffer_get_fixed_int8(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int8_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

int16_t tud_cdc_buffer_get_fixed_int16(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int16_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

int32_t tud_cdc_buffer_get_fixed_int32(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	int32_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

uint8_t tud_cdc_buffer_get_fixed_uint8(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint8_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

uint16_t tud_cdc_buffer_get_fixed_uint16(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint16_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

uint32_t tud_cdc_buffer_get_fixed_uint32(uint8_t target, bool flush)
{
	if (flush)
	{
		tud_cdc_buffer_flush();
	}

	uint32_t value = 0;
	uint8_t length = 0;
	while (length < target)
	{
		int32_t c = tud_cdc_buffer_read();
		if (c == -1)
		{
			continue;
		}

		if (c >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			length++;
			continue;
		}
	}

	return value;
}

bool is_printable(uint8_t c)
{
	const char list[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ";
	for (size_t i = 0; i < sizeof(list); i++)
	{
		if (c == list[i])
		{
			return true;
		}
	}
	return false;
}

uint8_t gencrc(uint8_t *data, size_t len)
{
	uint8_t crc = 0xff;
	size_t i, j;
	for (i = 0; i < len; i++)
	{
		crc ^= data[i];
		for (j = 0; j < 8; j++)
		{
			if ((crc & 0x80) != 0)
				crc = (uint8_t)((crc << 1) ^ 0x31);
			else
				crc <<= 1;
		}
	}
	return crc;
}

int16_t hstr_to_uint16(uint8_t *buf)
{
	uint8_t val = 0;

	if ((buf[0] >= 'a' && buf[0] <= 'f'))
	{
		buf[0] -= 'a';
		buf[0] += 10;
	}
	else if ((buf[1] >= 'A' && buf[1] <= 'F'))
	{
		buf[0] -= 'A';
		buf[0] += 10;
	}
	else if (buf[0] >= '0' && buf[0] <= '9')
	{
		buf[0] -= '0';
	}
	else
	{
		return -1;
	}
	val = buf[0] * 16;

	if ((buf[1] >= 'a' && buf[1] <= 'f'))
	{
		buf[1] -= 'a';
		buf[1] += 10;
	}
	else if ((buf[1] >= 'A' && buf[1] <= 'F'))
	{
		buf[1] -= 'A';
		buf[1] += 10;
	}
	else if (buf[1] >= '0' && buf[1] <= '9')
	{
		buf[1] -= '0';
	}
	else
	{
		return -1;
	}
	val |= buf[1];

	return val;
}

uint8_t uint16_to_hstr(uint64_t val, uint8_t shift)
{
	uint8_t buf = (uint8_t)((val >> shift) & 0x0F);
	if (buf < 10)
	{
		buf += '0';
	}
	else
	{
		buf += 'A' - 10;
	}
	return buf;
}

uint32_t init_pwm(uint8_t pin)
{
	gpio_init(pin);
	gpio_set_function(pin, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(pin);
	uint channel_num = pwm_gpio_to_channel(pin);

	uint32_t clock = clock_get_hz(clk_sys);

	uint32_t div = clock / 50 / 4096 + (clock % (50 * 4096) != 0);
	if (div / 16 == 0)
	{
		div = 16;
	}
	pwm_set_clkdiv_int_frac(slice_num, div / 16, div & 0xF);

	uint32_t wrap = clock * 16 / div / 50 - 1;
	pwm_set_wrap(slice_num, wrap);

	pwm_set_enabled(slice_num, true);

	return wrap;
}
