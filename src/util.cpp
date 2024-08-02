#include "defs.h"
#include "util.h"

void uart_rx_clear(uart_inst_t *uart)
{
	while (uart_is_readable(uart))
	{
		uart_getc(uart);
	}
}

void __attribute__((noreturn)) __printflike(1, 0) userpanic(const char *fmt, ...)
{
	uart_puts(PANIC_UART, "\n*** PANIC ***\n");

	if (fmt)
	{
		va_list args1, args2;
		va_start(args1, fmt);
		va_copy(args2, args1);

		size_t len = vsnprintf(NULL, 0, fmt, args1) + 1;
		va_end(args1);

		char *message = (char *)malloc(len);
		vsprintf(message, fmt, args1);
		va_end(args2);

		uart_puts(PANIC_UART, message);
		free(message);
		uart_puts(PANIC_UART, "\n");
	}

	uart_puts(PANIC_UART, "Press Ctrl+R to reset...\n");
	uart_puts(PANIC_UART, "Press Ctrl+C to upload...\n");

	uart_rx_clear(PANIC_UART);

	while (true)
	{
		if (uart_is_readable(PANIC_UART))
		{
			char c = uart_getc(PANIC_UART);

			if (c == 0x03) // Ctrl+C
			{
				reset_usb_boot(0, 0);
				while (true)
					;
			}

			if (c == 0x12) // Ctrl+R
			{
				watchdog_enable(1, 1);
				while (true)
					;
			}
		}

		sleep_ms(10);
	}
}