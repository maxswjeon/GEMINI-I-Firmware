#include "defs.h"
#include "util.h"

void __attribute__((noreturn)) __printflike(1, 0) userpanic(const char *fmt, ...)
{
	va_list args1, args2;
	va_start(args1, fmt);
	va_copy(args2, args1);

	size_t len = vsnprintf(NULL, 0, fmt, args1) + 1;
	va_end(args1);

	char *message = (char *)malloc(len);
	vsprintf(message, fmt, args2);
	va_end(args2);

	uart_printf(PANIC_UART, "\n\npanic: %s\n", message);

	uart_puts(PANIC_UART, "Press Ctrl+C to reset...\n");
	uart_puts(PANIC_UART, "Press Ctrl+D to upload...\n");

	uart_rx_clear(PANIC_UART);

	multicore_reset_core1();

	while (true)
	{
		if (uart_is_readable(PANIC_UART))
		{
			char c = uart_getc(PANIC_UART);

			if (c == 0x03) // Ctrl+C
			{
				watchdog_enable(1, 1);
				while (true)
					;
			}

			if (c == 0x04) // Ctrl+D
			{
				reset_usb_boot(0, 0);
				while (true)
					;
			}
		}

		sleep_ms(10);
	}
}