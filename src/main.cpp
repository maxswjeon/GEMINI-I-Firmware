#include <stddef.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/bootrom.h>

#include <hardware/watchdog.h>

#include "sensor.h"
#include "util.h"

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_cdc.h"

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

	uint64_t last_print = 0;
	int32_t c;

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

	// Ctrl-T (0x14) to launch RTC setup
	if (c == 0x14)
	{
#ifdef RTC_CONFIGURE_SUPPORT
		tud_cdc_printf("Launching RTC Setup\r\n");
		multicore_launch_core1(rtc_configure);
#else
		tud_cdc_printf("RTC Setup not supported\r\n");
#endif
	}
	else
	{
		multicore_launch_core1(sensor_main);
	}

	while (true)
	{
		tud_cdc_write_flush();
		tud_task();
		tuh_task();
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
