#include <math.h>
#include <stddef.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/bootrom.h>
#include <pico/cyw43_arch.h>

#include <hardware/adc.h>
#include <hardware/pwm.h>
#include <hardware/flash.h>
#include <hardware/watchdog.h>

#include "util.h"
#include "flash.h"
#include "sensor.h"
#include "command.h"

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_cdc.h"

state_t state;

int main()
{
	board_init();
	usbd_serial_init();

	tuh_init(BOARD_TUH_RHPORT);
	tud_init(BOARD_TUD_RHPORT);

	if (board_init_after_tusb)
	{
		board_init_after_tusb();
	}

	cyw43_arch_init();
	state_init(state);

	adc_init();

	uart_init(uart0, 9600);
	gpio_set_function(0, GPIO_FUNC_UART);
	gpio_set_function(1, GPIO_FUNC_UART);

	gpio_init(13);
	gpio_set_dir(13, GPIO_OUT);

	gpio_init(22);
	gpio_set_dir(22, GPIO_IN);

	gpio_init(27);
	gpio_set_dir(27, GPIO_IN);

	adc_gpio_init(26);
	// adc_gpio_init(27);

	if (state.motor1_state)
	{
		pwm_set_chan_level(16, 0, POS_OPEN(state.motor1_wrap));
	}
	else
	{
		pwm_set_chan_level(16, 0, POS_CLOSE(state.motor1_wrap));
	}

	if (state.motor2_state)
	{
		pwm_set_chan_level(17, 0, POS_OPEN(state.motor2_wrap));
	}
	else
	{
		pwm_set_chan_level(17, 0, POS_CLOSE(state.motor2_wrap));
	}

#if DEBUG_PAUSE
	uint64_t last_print = 0;
	int32_t c;

	tud_cdc_read_flush();
	while (true)
	{
		if (time_us_64() - last_print > 1000 * 1000)
		{
			tud_cdc_printf("Press any key to continue\r\n");
			last_print = time_us_64();
		}

		tud_cdc_write_flush();
		tud_task();
		tuh_task();

		int32_t c = tud_cdc_buffer_read();
		if (c != -1)
		{
			break;
		}
	}

	// R or r to launch RTC setup
	if (c == 'R' || c == 'r')
	{
#if RTC_CONFIGURE_SUPPORT
		tud_cdc_printf("Launching RTC Setup\r\n");
		multicore_reset_core1();
		multicore_launch_core1(rtc_configure);
#else
		tud_cdc_printf("RTC Setup not supported\r\n");
#endif
	}
	else
	{
		multicore_reset_core1();
		multicore_launch_core1(sensor_main);
	}
#else
	multicore_launch_core1(sensor_main);
#endif

	tud_cdc_write_str("Core 1 Started\r\n");
	tud_cdc_write_flush();
	tud_task();

	uint8_t buf[10];
	uint16_t buf_index = 0;
	uint8_t data[4] = {
		0,
	};
	uint8_t crc = 0;

	while (true)
	{
		tud_cdc_write_flush();
		tud_task();
		tuh_task();

		if (time_us_64() - imu_last_update > 3 * 1000 * 1000)
		{
			tud_cdc_write_str("Core 1 Stall - Resetting\r\n");
			multicore_reset_core1();
			multicore_launch_core1(sensor_main);
		}

		if (uart_is_readable(uart0))
		{
			process_command(state);
		}

		double accl_xy = sqrt(imu_accel[0] * imu_accel[0] + imu_accel[1] * imu_accel[1]);
		double accl_xyz = sqrt(accl_xy * accl_xy + imu_accel[2] * imu_accel[2]);

		if (accl_xyz > 2.0)
		{
			gpio_put(13, 1);
		}
	}
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

void tud_cdc_rx_cb(uint8_t itf)
{
	(void)itf; // no-op

	char buf[512];
	uint32_t count = tud_cdc_read(buf, sizeof(buf));

	for (int i = 0; i < count; ++i)
	{
		if (buf[i] == 0x03) // Ctrl-C
		{
			watchdog_enable(0, 0);
			while (1)
				;
		}

		if (buf[i] == 0x04) // Ctrl-D
		{
			reset_usb_boot(0, 0);
			while (1)
				;
		}

		tud_cdc_buffer_write(buf[i]);
		tud_cdc_write_char(buf[i]);
	}
}

void tuh_mount_cb(uint8_t dev_addr)
{
	tud_cdc_printf("A device with address %d is mounted\r\n", dev_addr);
}

void tuh_umount_cb(uint8_t dev_addr)
{
	tud_cdc_printf("A device with address %d is unmounted \r\n", dev_addr);
}
