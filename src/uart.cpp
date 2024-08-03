#include "uart.h"

void uart_rx_clear(uart_inst_t *uart)
{
	while (uart_is_readable(uart))
	{
		uart_getc(uart);
	}
}

int __printflike(2, 3) uart_printf(uart_inst_t *uart, const char *fmt, ...)
{
	if (!uart)
	{
		return 0;
	}

	va_list args1, args2;
	va_start(args1, fmt);
	va_copy(args2, args1);

	size_t len = vsnprintf(NULL, 0, fmt, args1) + 1;
	va_end(args1);

	char *message = (char *)malloc(len);
	vsprintf(message, fmt, args2);
	va_end(args2);

	for (size_t i = 0; i < len; i++)
	{
		uart_putc(uart, message[i]);
	}

	free(message);

	return len;
}
