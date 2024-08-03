# GEMINI-I Firmware

This is the Firmware for GEMINI-I

## IMU(BHI260AP)

### Hardware Connections

The IMU(BHI260AP) is connected through SPI0.

- SCK: GPIO2
- MOSI: GPIO3
- MISO: GPIO4
- CSn: GPIO5
- RESET: GPIO10

### Firmware

- [SensorAPI](https://github.com/boschsensortec/BHY2_SensorAPI) from Bosch Sensortec (BSD-3-Clause)
- [Fuser2 Firmware](https://www.bosch-sensortec.com/products/smart-sensor-systems/bhi260ap/#documents) from Bosch Sensortec, Proprietary

The included firmware are provided by Bosch Sensortech, All Rights Reserved
