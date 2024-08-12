#include "defs.h"
#include "sensor.h"
#include "util.h"

#include <pico/rand.h>
#include <pico/cyw43_arch.h>

#include <hardware/i2c.h>
#include <hardware/adc.h>

#include "tusb.h"

// #include IMU_FIRMWARE_H
// #define IMU_FIRMWARE_LEN CAT(IMU_FIRMWARE, _len)

uint64_t imu_last_update = 0;
int16_t imu_accel[3] = {
	0,
};
int16_t imu_gyro[3] = {
	0,
};
int16_t temp = 0;
uint16_t adc0_value = 0;

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
	imu_last_update = time_us_64();

	rtc_setup();

	imu_setup();

	uint64_t last_print = 0;
	bool state = false;
	while (true)
	{
		imu_last_update = time_us_64();

		mpu6050_read_raw(imu_accel, imu_gyro, &temp);

		adc_select_input(0);
		adc0_value = adc_read();

		if (time_us_64() - last_print > 1000 * 1000)
		{
			state != state;
			last_print = time_us_64();
			cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
		}
	}
}

void imu_setup()
{
	imu_inst_setup();
	imu_reset();
}

void imu_inst_setup()
{
	// BHI360
	// spi_init(IMU_SPI_INST, 1000 * 1000);
	// spi_set_format(IMU_SPI_INST, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	// gpio_set_function(IMU_SPI_MISO, GPIO_FUNC_SPI);
	// gpio_set_function(IMU_SPI_MOSI, GPIO_FUNC_SPI);
	// gpio_set_function(IMU_SPI_SCK, GPIO_FUNC_SPI);
	//
	// gpio_init(IMU_SPI_CSn);
	// gpio_set_dir(IMU_SPI_CSn, GPIO_OUT);
	// gpio_put(IMU_SPI_CSn, 1);
	//
	// gpio_init(IMU_RESET);
	// gpio_set_dir(IMU_RESET, GPIO_OUT);
	//
	// bi_decl(bi_4pins_with_func(IMU_SPI_MISO, IMU_SPI_MOSI, IMU_SPI_SCK, IMU_SPI_CSn, GPIO_FUNC_SPI));
	// bi_decl(bi_1pin_with_name(IMU_RESET, "IMU Reset"));

	// MPU6050
	i2c_init(i2c0, 400 * 1000);
	gpio_set_function(20, GPIO_FUNC_I2C);
	gpio_set_function(21, GPIO_FUNC_I2C);
	gpio_pull_up(20);
	gpio_pull_up(21);
	bi_decl(bi_2pins_with_func(20, 21, GPIO_FUNC_I2C));
}

bool imu_reset()
{
	uint8_t buf[] = {0x6B, 0x80};
	i2c_write_blocking(i2c_default, 0x68, buf, 2, false);
	return true;
}

void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp)
{
	// For this particular device, we send the device the register we want to read
	// first, then subsequently read from the device. The register is auto incrementing
	// so we don't need to keep sending the register we want, just the first.

	uint8_t buffer[6];

	// Start reading acceleration registers from register 0x3B for 6 bytes
	uint8_t val = 0x3B;
	i2c_write_blocking(i2c_default, 0x68, &val, 1, true); // true to keep master control of bus
	i2c_read_blocking(i2c_default, 0x68, buffer, 6, false);

	for (int i = 0; i < 3; i++)
	{
		accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
	}

	// Now gyro data from reg 0x43 for 6 bytes
	// The register is auto incrementing on each read
	val = 0x43;
	i2c_write_blocking(i2c_default, 0x68, &val, 1, true);
	i2c_read_blocking(i2c_default, 0x68, buffer, 6, false); // False - finished with bus

	for (int i = 0; i < 3; i++)
	{
		gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
		;
	}

	// Now temperature from reg 0x41 for 2 bytes
	// The register is auto incrementing on each read
	val = 0x41;
	i2c_write_blocking(i2c_default, 0x68, &val, 1, true);
	i2c_read_blocking(i2c_default, 0x68, buffer, 2, false); // False - finished with bus

	*temp = buffer[0] << 8 | buffer[1];
}