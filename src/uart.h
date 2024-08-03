#ifndef __UART_H
#define __UART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <hardware/uart.h>

void uart_rx_clear(uart_inst_t *uart);
int __printflike(2, 3) uart_printf(uart_inst_t *uart, const char *fmt, ...);
char *__printflike(2, 3) uart_sprintf(uart_inst_t *uart, const char *fmt, ...);

#endif // __UART_H