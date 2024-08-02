#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <pico/multicore.h>
#include <pico/bootrom.h>
#include <hardware/uart.h>
#include <hardware/watchdog.h>

void uart_rx_clear(uart_inst_t *uart);
void __attribute__((noreturn)) __printflike(1, 0) userpanic(const char *fmt, ...);

#endif // __UTIL_H