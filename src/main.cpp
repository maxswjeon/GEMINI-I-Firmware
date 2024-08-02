#include <stddef.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include "util.h"

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
	uart_init(uart0, 115200);
	gpio_set_function(0, GPIO_FUNC_UART);
	gpio_set_function(1, GPIO_FUNC_UART);
	uart_set_hw_flow(uart0, false, false);
	uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
	uart_set_fifo_enabled(uart0, true);
	bi_decl(bi_2pins_with_func(0, 1, GPIO_FUNC_UART));

	// uart_init(uart1, 115200);
	// gpio_set_function(8, GPIO_FUNC_UART);
	// gpio_set_function(9, GPIO_FUNC_UART);

	// uart_set_hw_flow(uart1, false, false);
	// uart_set_format(uart1, 8, 1, UART_PARITY_NONE);

	uart_rx_clear(uart0);

	while (true)
	{
		uart_puts(uart0, "Press any key to start...\n");

		if (uart_is_readable(uart0))
		{
			uart_getc(uart0);
			break;
		}

		sleep_ms(1000);
	}

	userpanic("main exits");
}