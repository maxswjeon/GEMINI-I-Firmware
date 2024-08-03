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

#include "uart.h"

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define CAT_2(X, Y) X##Y
#define CAT(X, Y) CAT_2(X, Y)

void __attribute__((noreturn)) __printflike(1, 2) userpanic(const char *fmt, ...);

#endif // __UTIL_H