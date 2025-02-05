#ifndef __UTIL_H
#define __UTIL_H

#include "defs.h"

#include <pico/stdlib.h>

#include <stddef.h>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define CAT_2(X, Y) X##Y
#define CAT(X, Y) CAT_2(X, Y)

#define SERIAL_BUFFER_SIZE 2048

extern int32_t tud_cdc_buffer[SERIAL_BUFFER_SIZE];

#ifdef __cplusplus
extern "C"
{
#endif

	void __attribute__((noreturn)) __printflike(1, 0) tusb_panic(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#if DEBUG_UART
#include <stdio.h>
#define dprintf(...) printf(__VA_ARGS__)
#elif DEBUG_USB
#define dprintf(...) tud_cdc_printf(__VA_ARGS__)
#else
#define dprintf(...) void(0)
#endif

void __printflike(1, 0) tud_cdc_printf(const char *fmt, ...);

int8_t tud_cdc_buffer_get_int8(bool flush = true);
int16_t tud_cdc_buffer_get_int16(bool flush = true);
int32_t tud_cdc_buffer_get_int32(bool flush = true);
uint8_t tud_cdc_buffer_get_uint8(bool flush = true);
uint16_t tud_cdc_buffer_get_uint16(bool flush = true);
uint32_t tud_cdc_buffer_get_uint32(bool flush = true);

int8_t tud_cdc_buffer_get_fixed_int8(uint8_t length, bool flush = true);
int16_t tud_cdc_buffer_get_fixed_int16(uint8_t length, bool flush = true);
int32_t tud_cdc_buffer_get_fixed_int32(uint8_t length, bool flush = true);
uint8_t tud_cdc_buffer_get_fixed_uint8(uint8_t length, bool flush = true);
uint16_t tud_cdc_buffer_get_fixed_uint16(uint8_t length, bool flush = true);
uint32_t tud_cdc_buffer_get_fixed_uint32(uint8_t length, bool flush = true);

void tud_cdc_buffer_init();
void tud_cdc_buffer_flush();
int32_t tud_cdc_buffer_read();
void tud_cdc_buffer_write(int32_t c);

bool is_printable(uint8_t c);
uint8_t gencrc(uint8_t *data, size_t len);
int16_t hstr_to_uint16(uint8_t *buf);
uint8_t uint16_to_hstr(uint64_t val, uint8_t shift);

uint32_t init_pwm(uint8_t pin);

#endif // __UTIL_H