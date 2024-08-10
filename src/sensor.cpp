#include "defs.h"
#include "sensor.h"
#include "util.h"

#include <pico/rand.h>

#include <hardware/i2c.h>

#include "tusb.h"

#include IMU_FIRMWARE_H
#define IMU_FIRMWARE_LEN CAT(IMU_FIRMWARE, _len)

void print_status(uint8_t status);
bool poll_status(uint8_t check_ready, uint8_t check_verify, uint64_t print_interval_ms = 1000, uint64_t timeout_ms = 5000);

void rtc_setup()
{
	i2c_init(i2c1, 100 * 1000);
	gpio_set_function(6, GPIO_FUNC_I2C);
	gpio_set_function(7, GPIO_FUNC_I2C);

	uint8_t buf[7] = {
		0,
	};
	i2c_write_blocking(i2c1, 0x68, buf, 1, true);
	i2c_read_blocking(i2c1, 0x68, buf, 7, false);

	buf[0] = buf[0] & 0x7F; // sec
	buf[1] = buf[1] & 0x7F; // min
	buf[2] = buf[2] & 0x3F; // hour
	buf[3] = buf[3] & 0x07; // week
	buf[4] = buf[4] & 0x3F; // day
	buf[5] = buf[5] & 0x1F; // mouth

	tud_cdc_printf("\r\nRTC: 20%02x-%02x-%02x %02x:%02x:%02x\r\n", buf[6], buf[5], buf[4], buf[2], buf[1], buf[0]);
}

#ifdef RTC_CONFIGURE_SUPPORT
/**
 * @brief           Library function that takes an 8 bit unsigned integer and converts into
 *  Binary Coded Decimal number that can be written to DS3231 registers.
 *
 * @param[in] data  Number to be converted.
 * @return          Number in BCD form.
 */
uint8_t bin_to_bcd(const uint8_t data)
{
	uint8_t ones_digit = (uint8_t)(data % 10);
	uint8_t twos_digit = (uint8_t)(data - ones_digit) / 10;
	return ((twos_digit << 4) + ones_digit);
}

void rtc_configure()
{
	rtc_setup();

	while (true)
	{
		uint8_t buf[7] = {
			0,
		};
		i2c_write_blocking(i2c1, 0x68, buf, 1, true);
		i2c_read_blocking(i2c1, 0x68, buf, 7, false);

		buf[0] = buf[0] & 0x7F; // sec
		buf[1] = buf[1] & 0x7F; // min
		buf[2] = buf[2] & 0x3F; // hour
		buf[3] = buf[3] & 0x07; // week
		buf[4] = buf[4] & 0x3F; // day
		buf[5] = buf[5] & 0x1F; // mouth

		tud_cdc_printf("\r\nRTC Set: 20%02x-%02x-%02x %02x:%02x:%02x\r\n", buf[6], buf[5], buf[4], buf[2], buf[1], buf[0]);

		tud_cdc_buffer_flush();

		uint8_t year, month, date, hour, minute, second;
		year = tud_cdc_buffer_get_fixed_uint16(4) % 100;
		buf[6] = bin_to_bcd(year % 100);

		uint8_t _month = tud_cdc_buffer_get_fixed_uint8(2);
		month = _month;
		buf[5] = bin_to_bcd(month);

		uint8_t _date = tud_cdc_buffer_get_fixed_uint8(2);
		date = _date;
		buf[4] = bin_to_bcd(date);

		hour = tud_cdc_buffer_get_fixed_uint8(2);
		buf[2] = bin_to_bcd(hour);

		minute = tud_cdc_buffer_get_fixed_uint8(2);
		buf[1] = bin_to_bcd(minute);

		second = tud_cdc_buffer_get_fixed_uint8(2);
		buf[0] = bin_to_bcd(second);

		buf[3] = bin_to_bcd((_date += _month < 3 ? year-- : year - 2, 23 * _month / 9 + _date + 4 + year / 4 - year / 100 + year / 400) % 7);
		tud_cdc_printf("Weekday: %d\r\n", buf[3]);

		if (month < 1 || month > 12)
		{
			tud_cdc_printf("Invalid Month: %d\r\n", month);
			continue;
		}

		if (date < 1 || date > 31)
		{
			tud_cdc_printf("Invalid Date: %d\r\n", date);
			continue;
		}

		if (hour < 0 || hour > 23)
		{
			tud_cdc_printf("Invalid Hour: %d\r\n", hour);
			continue;
		}

		if (minute < 0 || minute > 59)
		{
			tud_cdc_printf("Invalid Minute: %d\r\n", minute);
			continue;
		}

		if (second < 0 || second > 59)
		{
			tud_cdc_printf("Invalid Second: %d\r\n", second);
			continue;
		}

		tud_cdc_printf("\r\nValid time set: 20%02x-%02x-%02x %02x:%02x:%02x\r\n", year, month, date, hour, minute, second);
		tud_cdc_printf("Waiting for RTC to set time...\r\n");

		tud_cdc_buffer_flush();
		while (tud_cdc_buffer_read() != -1)
		{
			tight_loop_contents();
		}

		for (int i = 0; i < 7; i++)
		{
			uint8_t temp[2];
			temp[0] = i;
			temp[1] = buf[i];
			tud_cdc_printf("Setting %d: 0x%02X\r\n", i, buf[i]);
			i2c_write_blocking(i2c1, 0x68, temp, 2, false);
		}
	}
}
#endif

void sensor_main()
{
	rtc_setup();
	imu_setup();
}

void imu_setup()
{
	imu_inst_setup();
	if (!imu_reset())
	{
		panic("imu reset");
	}
	tud_cdc_printf("[INFO] IMU Reset Success\r\n");

	// Is it bootable?
	// sleep_ms(150);
	// if (poll_status(WAIT_TRUE, WAIT_TRUE))
	// {
	// 	uint8_t status = imu_read_reg(0x25);
	// 	print_status(status);
	// 	return;
	// }

	// Reset the IMU - force to Host Boot Mode
	imu_reset_soft();
	imu_reset_soft();
	imu_reset_soft();

	tud_cdc_printf("[INFO] Setting Host Control\r\n");
	imu_write_reg(0x16, 0x00);

	tud_cdc_printf("[INFO] Setting Host Interface\r\n");
	imu_write_reg(0x07, 0x00);

	tud_cdc_printf("[INFO] Setting Turbo Mode\r\n");
	imu_write_reg(0x05, 0x00);

	// Wait T_boot_bl time
	sleep_ms(10);

	if (!poll_status(WAIT_TRUE, WAIT_NONE))
	{
		panic("imu status: interface");
	}

	uint8_t imu_identifier = imu_read_reg(0x1C);
	uint8_t imu_revision = imu_read_reg(0x1D);
	uint8_t rom_revision_lsb = imu_read_reg(0x1E);
	uint8_t rom_revision_msb = imu_read_reg(0x1F);

	tud_cdc_printf("[INFO] IMU Identifier: 0x%02X\r\n", imu_identifier);
	tud_cdc_printf("[INFO] IMU Revision: 0x%02X\r\n", imu_revision);
	tud_cdc_printf("[INFO] ROM Revision: 0x%02X%02X\r\n", rom_revision_msb, rom_revision_lsb);

	tud_cdc_printf("[INFO] Loading Firmware\r\n");

	uint8_t command = 0x00;
	uint8_t clear[4] = {0, 0, 0, 0};

	uint8_t payload[4];
	payload[0] == 0x02;
	payload[1] == 0x00;

	uint16_t fw_len = IMU_FIRMWARE_LEN / 4;
	payload[2] = fw_len & 0xFF;
	payload[3] = (fw_len >> 8) & 0xFF;

	const char firmware_name[] = STRINGIFY(IMU_FIRMWARE);
	tud_cdc_printf("[INFO] Firmware Name: %s\r\n", firmware_name);
	tud_cdc_printf("[INFO] Firmware Length: %d\r\n", IMU_FIRMWARE_LEN);

	gpio_put(IMU_SPI_CSn, 0);
	spi_write_blocking(IMU_SPI_INST, &command, 1);
	spi_write_blocking(IMU_SPI_INST, payload, 4);
	spi_write_blocking(IMU_SPI_INST, IMU_FIRMWARE, IMU_FIRMWARE_LEN);
	gpio_put(IMU_SPI_CSn, 1);

	uint8_t count = 0;
	while (!poll_status(WAIT_FALSE, WAIT_TRUE, 1000, 20 * 1000))
	{
		count += 4;
		gpio_put(IMU_SPI_CSn, 0);
		spi_write_blocking(IMU_SPI_INST, clear, 4);
		gpio_put(IMU_SPI_CSn, 1);

		tud_cdc_printf("[INFO] Padding %d bytes\r\n", count);
	}

	tud_cdc_printf("[INFO] Starting Firmware\r\n");
	payload[0] = 0x03;
	payload[1] = 0x00;
	payload[2] = 0x00;
	payload[3] = 0x00;

	gpio_put(IMU_SPI_CSn, 0);
	spi_write_blocking(IMU_SPI_INST, &command, 1);
	spi_write_blocking(IMU_SPI_INST, payload, 4);
	gpio_put(IMU_SPI_CSn, 1);

	poll_status(WAIT_FALSE, WAIT_NONE);
	poll_status(WAIT_TRUE, WAIT_NONE);

	tud_cdc_printf("[INFO] Firmware Loaded\r\n");
}

void imu_inst_setup()
{
	spi_init(IMU_SPI_INST, 1000 * 1000);
	spi_set_format(IMU_SPI_INST, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	gpio_set_function(IMU_SPI_MISO, GPIO_FUNC_SPI);
	gpio_set_function(IMU_SPI_MOSI, GPIO_FUNC_SPI);
	gpio_set_function(IMU_SPI_SCK, GPIO_FUNC_SPI);

	gpio_init(IMU_SPI_CSn);
	gpio_set_dir(IMU_SPI_CSn, GPIO_OUT);
	gpio_put(IMU_SPI_CSn, 1);

	gpio_init(IMU_RESET);
	gpio_set_dir(IMU_RESET, GPIO_OUT);

	bi_decl(bi_4pins_with_func(IMU_SPI_MISO, IMU_SPI_MOSI, IMU_SPI_SCK, IMU_SPI_CSn, GPIO_FUNC_SPI));
	bi_decl(bi_1pin_with_name(IMU_RESET, "IMU Reset"));
}

uint8_t imu_read_reg(uint8_t address)
{
	uint8_t buffer;

	address |= 0x80; // Read command
	gpio_put(IMU_SPI_CSn, 0);
	spi_write_blocking(IMU_SPI_INST, &address, 1);
	spi_read_blocking(IMU_SPI_INST, 0, &buffer, 1);
	gpio_put(IMU_SPI_CSn, 1);
	return buffer;
}

void imu_write_reg(uint8_t address, uint8_t value)
{
	gpio_put(IMU_SPI_CSn, 0);
	spi_write_blocking(IMU_SPI_INST, &address, 1);
	spi_write_blocking(IMU_SPI_INST, &value, 1);
	gpio_put(IMU_SPI_CSn, 1);
}

void imu_write_data(uint8_t reg, uint8_t *data, uint8_t len)
{
	uint8_t buffer[257];
	buffer[0] = reg;
	memcpy(&buffer[1], data, len);

	gpio_put(IMU_SPI_CSn, 0);
	spi_write_blocking(IMU_SPI_INST, buffer, len + 1);
	gpio_put(IMU_SPI_CSn, 1);
}

bool imu_reset()
{
	int count = 0;

	imu_reset_soft();

	if (!poll_status(WAIT_TRUE, WAIT_NONE))
	{
		panic("imu status: interface");
	}

	while (count < IMU_SENSOR_TRIES)
	{
		count++;

		uint8_t sensor_id = imu_read_reg(0x2B);

#ifndef IMU_SENSOR_ID2
		if (sensor_id == IMU_SENSOR_ID)
		{
			tud_cdc_printf("[INFO] Sensor ID: 0x%02X\r\n", sensor_id);
			return true;
		}
		tud_cdc_printf("[INFO] Sensor ID: 0x%02X, expected 0x%02X (%d / %d)\r\n", sensor_id, IMU_SENSOR_ID, count, IMU_SENSOR_TRIES);
#else
		if (sensor_id == IMU_SENSOR_ID || sensor_id == IMU_SENSOR_ID2)
		{
			tud_cdc_printf("[INFO] Sensor ID: 0x%X\r\n", sensor_id);
			return true;
		}
		tud_cdc_printf("[INFO] Sensor ID: 0x%02X, expected 0x%02X or 0x%02X (%d / %d)\r\n", sensor_id, IMU_SENSOR_ID, IMU_SENSOR_ID2, count, IMU_SENSOR_TRIES);
#endif
	}

	if (count == IMU_SENSOR_TRIES)
	{
		tud_cdc_printf("[INFO] Sensor Soft Reset Failed, trying Hard Reset...\r\n");
	}

	imu_reset_hard();

	if (!poll_status(WAIT_TRUE, WAIT_NONE))
	{
		panic("imu status: interface");
	}

	imu_reset_soft();

	count = 0;
	while (count < IMU_SENSOR_TRIES)
	{
		count++;

		uint8_t sensor_id = imu_read_reg(0x2B);
#ifndef IMU_SENSOR_ID2 if (sensor_id == IMU_SENSOR_ID)
		{
			tud_cdc_printf("[INFO] Sensor ID: 0x%02X\r\n", sensor_id);
			return true;
		}
		tud_cdc_printf("[INFO] Sensor ID: 0x%02X, expected 0x%02X (%d / %d)\r\n", sensor_id, IMU_SENSOR_ID, count, IMU_SENSOR_TRIES);
#else
		if (sensor_id == IMU_SENSOR_ID || sensor_id == IMU_SENSOR_ID2)
		{
			tud_cdc_printf("[INFO] Sensor ID: 0x%X\r\n", sensor_id);
			return true;
		}
		tud_cdc_printf("[INFO] Sensor ID: 0x%02X, expected 0x%02X or 0x%02X (%d / %d)\r\n", sensor_id, IMU_SENSOR_ID, IMU_SENSOR_ID2, count, IMU_SENSOR_TRIES);
#endif
	}

	return false;
}

void imu_reset_hard()
{
	gpio_put(IMU_RESET, 0);
	sleep_ms(10);
	gpio_put(IMU_RESET, 1);
	sleep_ms(10);
}

void imu_reset_soft()
{
	imu_write_reg(0x14, 0x01);
	sleep_ms(100);
}

void print_status(uint8_t status)
{
	bool interface_ready = (status & 0x10) != 0;
	bool firmware_loading = (status & 0x20) == 0;
	bool firmware_verify = (status & 0x40) == 0;
	bool firmware_running = (status & 0x80) == 0;

	tud_cdc_printf("[INFO] \r\n");
	tud_cdc_printf("[INFO] Boot Status: 0x%02X\r\n", status);

	if (interface_ready)
	{
		tud_cdc_printf("[INFO] \tInterface: Ready\r\n");
	}
	else
	{
		tud_cdc_printf("[INFO] \tInterface: Not Ready\r\n");
	}

	if (firmware_loading)
	{
		tud_cdc_printf("[INFO] \tFirmware: Verification in Progress\r\n");
	}
	else if (firmware_verify)
	{
		tud_cdc_printf("[INFO] \tFirmware: Verification Success\r\n");
	}
	else
	{
		tud_cdc_printf("[INFO] \tFirmware: Verification Failed\r\n");
	}

	if (firmware_running)
	{
		tud_cdc_printf("[INFO] \tFirmware: Running\r\n");
	}
	else
	{
		tud_cdc_printf("[INFO] \tFirmware: Halted\r\n");
	}
}

bool poll_status(uint8_t check_ready, uint8_t check_verify, uint64_t print_interval_ms, uint64_t timeout_ms)
{
	uint64_t time_last_print = 0;
	uint64_t time_check_start = time_us_64();
	while (true)
	{
		uint8_t status = imu_read_reg(0x25);
		bool interface_ready = (status & 0x10) != 0;
		bool firmware_loading = (status & 0x20) == 0;
		bool firmware_verify = (status & 0x40) == 0;
		bool firmware_running = (status & 0x80) == 0;

		bool verified = !firmware_loading && firmware_verify;

		if (time_us_64() - time_last_print > print_interval_ms * 1000)
		{
			print_status(status);
			time_last_print = time_us_64();
		}

		if (time_us_64() - time_check_start > timeout_ms * 1000)
		{
			return false;
		}

		if (check_verify != WAIT_NONE && ((check_verify == WAIT_TRUE) == verified))
		{
			print_status(status);
			return true;
		}

		if (check_ready != WAIT_NONE && ((check_ready == WAIT_TRUE) == interface_ready))
		{
			print_status(status);
			return true;
		}
	}
}
