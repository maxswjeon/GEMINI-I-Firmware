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

// PC Module: E22-900T22U
// Green, Not Blinking: Transmission Mode
// Green, Blinking: Transmission in Progress
// Red, Not Blinking: Configuration Mode
// Red, Blinking: Configuration in Progress (Command Response)

// Targeted launch (Exact Match)
// Broadcast transmission (Unicast)

// Default parameters
// Address: 0x0000
// Channel: 0x12
// Baud: 9600
// Parity: 8N1

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

	multicore_launch_core1(sensor_main);

	while (true)
	{
		tuh_task();
		tud_task();
	}
}

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

		tud_cdc_write_char(buf[i]);
	}
	tud_cdc_write_flush();
}