#ifndef __DEFS_H
#define __DEFS_H

#define PANIC_UART uart0

#define IMU_SPI_INST spi0
#define IMU_SPI_SCK 2
#define IMU_SPI_MOSI 3
#define IMU_SPI_MISO 4
#define IMU_SPI_CSn 5
#define IMU_RESET 10
#define IMU_SENSOR_TRIES 5

#define IMU_SENSOR_ID 0x7A
// #define IMU_SENSOR_ID2
#define IMU_FIRMWARE_H "Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688.fw.h"
#define IMU_FIRMWARE Bosch_Shuttle3_BHI360_BMM350C_BMP580_BME688_fw
#define IMU_WORK_BUFFER_SIZE 2048

#define IMU_LIST_VIRT_SENSORS 0
#define IMU_LIST_PHYS_SENSORS 0

#define DEBUG_PAUSE 0
#define DEBUG_USB 0
#define DEBUG_UART 0
#define RTC_CONFIGURE_SUPPORT 1

#endif // __DEFS_H