#ifndef __UTIL_H
#define __UTIL_H

#include <pico/stdlib.h>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define CAT_2(X, Y) X##Y
#define CAT(X, Y) CAT_2(X, Y)

#ifdef __cplusplus
extern "C"
{
#endif

	void __attribute__((noreturn)) __printflike(1, 0) tusb_panic(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

void __printflike(1, 0) tud_cdc_printf(const char *fmt, ...);

#endif // __UTIL_H`