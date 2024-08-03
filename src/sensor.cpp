#include "defs.h"
#include "sensor.h"
#include "util.h"
#include "uart.h"

#include IMU_FIRMWARE_H
#define IMU_FIRMWARE_LEN CAT(IMU_FIRMWARE, _len)

bool poll_status(uint8_t check_ready, uint8_t check_verify, uint64_t print_interval_ms = 1000, uint64_t timeout_ms = 5000);

void sensor_main()
{
	imu_setup();

	userpanic("core1 exit");
}

void imu_setup()
{
	imu_inst_setup();
	if (!imu_reset())
	{
		userpanic("imu reset");
	}

	uart_printf(uart0, "[INFO] IMU Reset Success\n");

	uint8_t imu_identifier = imu_read_reg(0x1C);
	uint8_t imu_revision = imu_read_reg(0x1D);
	uint8_t rom_revision_lsb = imu_read_reg(0x1E);
	uint8_t rom_revision_msb = imu_read_reg(0x1F);

	uart_printf(uart0, "[INFO] IMU Identifier: 0x%02X\n", imu_identifier);
	uart_printf(uart0, "[INFO] IMU Revision: 0x%02X\n", imu_revision);
	uart_printf(uart0, "[INFO] ROM Revision: 0x%02X%02X\n", rom_revision_msb, rom_revision_lsb);

	uart_printf(uart0, "[INFO] IMU Reset\n");
	imu_reset_soft();
	sleep_ms(100);

	uint8_t feature_status = imu_read_reg(0x24);
	uint8_t boot_status = imu_read_reg(0x25);

	uart_printf(uart0, "[INFO] Feature Status: 0x%02X\n", feature_status);
	uart_printf(uart0, "[INFO] Boot Status: 0x%02X\n", boot_status);

	// uart_printf(uart0, "[INFO] Setting Turbo Mode\n");
	// imu_write_reg(0x05, 0x00);

	// uart_printf(uart0, "[INFO] Setting Host Interrupt\n");
	// imu_write_reg(0x07, 0x00);

	// uart_printf(uart0, "[INFO] Setting Host Interface\n");
	// imu_write_reg(0x06, 0x00);

	// if (!poll_status(WAIT_TRUE, WAIT_NONE))
	// {
	// 	userpanic("imu status: interface");
	// }

	// uart_printf(uart0, "[INFO] Loading Firmware\n");
	// uint8_t payload[5];
	// payload[0] == 0x00;
	// payload[1] == 0x02;
	// payload[2] == 0x00;

	// uint16_t fw_len = IMU_FIRMWARE_LEN / 4;
	// payload[3] = fw_len & 0xFF;
	// payload[4] = (fw_len >> 8) & 0xFF;

	// const char firmware_name[] = STRINGIFY(IMU_FIRMWARE);
	// uart_printf(uart0, "[INFO] Firmware Name: %s\n", firmware_name);
	// uart_printf(uart0, "[INFO] Firmware Length: %d\n", IMU_FIRMWARE_LEN);

	// uint8_t clear[] = {0x00, 0x00, 0x00, 0x00, 0x00};

	// gpio_put(IMU_SPI_CSn, 0);
	// spi_write_blocking(IMU_SPI_INST, payload, 5);
	// spi_write_blocking(IMU_SPI_INST, IMU_FIRMWARE, IMU_FIRMWARE_LEN);
	// spi_write_blocking(IMU_SPI_INST, clear, 5);
	// gpio_put(IMU_SPI_CSn, 1);

	// if (!poll_status(WAIT_FALSE, WAIT_TRUE, 1000, 60 * 1000))
	// {
	// 	userpanic("imu status: firmware loading");
	// }

	// uart_printf(uart0, "[INFO] Starting Firmware\n");
	// payload[0] = 0x00;
	// payload[1] = 0x03;
	// payload[2] = 0x00;
	// payload[3] = 0x00;
	// payload[4] = 0x00;

	// gpio_put(IMU_SPI_CSn, 0);
	// spi_write_blocking(IMU_SPI_INST, payload, 5);
	// gpio_put(IMU_SPI_CSn, 1);

	// poll_status(WAIT_FALSE, WAIT_NONE);
	// poll_status(WAIT_TRUE, WAIT_NONE);

	// uart_printf(uart0, "[INFO] Firmware Loaded\n");
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
		userpanic("imu status: interface");
	}

	while (count < IMU_SENSOR_TRIES)
	{
		count++;

		uint8_t sensor_id = imu_read_reg(0x2B);
		if (sensor_id == 0x70 || sensor_id == 0xF0)
		{
			uart_printf(uart0, "[INFO] Sensor ID: 0x%X\n", sensor_id);
			return true;
		}
		uart_printf(uart0, "[INFO] Sensor ID: 0x%X, expected 0x70 or 0xF0 (%d / %d)\n", sensor_id, count, IMU_SENSOR_TRIES);
	}

	if (count == IMU_SENSOR_TRIES)
	{
		uart_printf(uart0, "[INFO] Sensor Soft Reset Failed, trying Hard Reset...\n");
	}

	imu_reset_hard();

	if (!poll_status(WAIT_TRUE, WAIT_NONE))
	{
		userpanic("imu status: interface");
	}

	imu_reset_soft();

	count = 0;
	while (count < IMU_SENSOR_TRIES)
	{
		count++;

		uint8_t sensor_id = imu_read_reg(0x2B);
		if (sensor_id == 0x70 || sensor_id == 0xF0)
		{
			uart_printf(uart0, "[INFO] Sensor ID: 0x%X\n", sensor_id);
			return true;
		}
		uart_printf(uart0, "[INFO] Sensor ID: 0x%X, expected 0x70 or 0xF0 (%d / %d)\n", sensor_id, count, IMU_SENSOR_TRIES);
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

	uart_printf(uart0, "[INFO] \n");
	uart_printf(uart0, "[INFO] Boot Status: 0x%02X\n", status);
	uart_printf(uart0, "[INFO] \tInterface: %s\n", interface_ready ? "Ready" : "Not Ready");
	if (firmware_loading)
	{
		uart_printf(uart0, "[INFO] \tFirmware: Verification in Progress\n");
	}
	else if (firmware_verify)
	{
		uart_printf(uart0, "[INFO] \tFirmware: Verification Success\n");
	}
	else
	{
		uart_printf(uart0, "[INFO] \tFirmware: Verification Failed\n");
	}
	uart_printf(uart0, "[INFO] \tFirmware State: %s\n\n", firmware_running ? "Running" : "Halted");
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
			uart_printf(uart0, "[ERROR] Timeout while waiting for the firmware to load\n");
			return false;
		}

		if (check_verify != WAIT_NONE && !firmware_loading && ((check_verify == WAIT_TRUE) == verified))
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
