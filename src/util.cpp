#include "defs.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <pico/platform.h>
#include <pico/multicore.h>
#include <pico/runtime.h>

#include "tusb.h"

bool core1_panic = false;
char *core1_panic_message = NULL;

void tud_cdc_vprintf(const char *fmt, va_list args)
{
	size_t len = vsnprintf(NULL, 0, fmt, args) + 1;
	char *message = (char *)malloc(len);
	vsprintf(message, fmt, args);

	tud_cdc_write_str(message);
	tud_cdc_write_flush();
}

void __printflike(1, 0) tud_cdc_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	tud_cdc_vprintf(fmt, args);
}

void __attribute__((noreturn)) __printflike(1, 0) tusb_panic(const char *fmt, ...)
{
	if (get_core_num() == 1)
	{
		va_list args1, args2;
		va_start(args1, fmt);

		size_t len = vsnprintf(NULL, 0, fmt, args1) + 1;
		va_end(args1);

		core1_panic_message = (char *)malloc(len);
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

		if (core1_panic_message)
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
			tud_task();
			tuh_task();
		}
	}
}