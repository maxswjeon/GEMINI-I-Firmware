#include "sensor.h"

#define FIFO_COMMAND(wakeup, nwakeup, len) ((wakeup << 6) | (nwakeup << 7) | len)

uint8_t fifo_command_table0[] = {
	/* 000 */ FIFO_COMMAND(1, 1, DATA_NONE), // Padding
	/* 001 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Accelerometer Passthrough
	/* 002 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 003 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Accelerometer Raw
	/* 004 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Accelerometer Corrected
	/* 005 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Accelerometer Offset
	/* 006 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Accelerometer Corrected
	/* 007 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Accelerometer Raw
	/* 008 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 009 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 010 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Gyroscope Passthrough
	/* 011 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 012 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Gyroscope Raw
	/* 013 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Gyroscope Corrected
	/* 014 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Gyroscope Offset
	/* 015 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Gyroscope Corrected
	/* 016 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Gyroscope Raw
	/* 017 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 018 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 019 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Magnetometer Passthrough
	/* 020 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 021 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Magnetometer Raw
	/* 022 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Magnetometer Corrected
	/* 023 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Magnetometer Offset
	/* 024 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Magnetometer Corrected
	/* 025 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Magnetometer Raw
	/* 026 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 027 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 028 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Gravity
	/* 029 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Gravity
	/* 030 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 031 */ FIFO_COMMAND(0, 1, DATA_VEC3), // Linear Acceleration
};

uint8_t fifo_command_table1[] = {
	/* 032 */ FIFO_COMMAND(1, 0, DATA_VEC3), // Linear Acceleration
	/* 033 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 034 */ FIFO_COMMAND(0, 1, DATA_QUAT), // Rotation Vector
	/* 035 */ FIFO_COMMAND(1, 0, DATA_QUAT), // Rotation Vector
	/* 036 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 037 */ FIFO_COMMAND(0, 1, DATA_QUAT), // Game Rotation Vector
	/* 038 */ FIFO_COMMAND(1, 0, DATA_QUAT), // Game Rotation Vector
	/* 039 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 040 */ FIFO_COMMAND(0, 1, DATA_QUAT), // Geomagnetic Rotation Vector
	/* 041 */ FIFO_COMMAND(1, 0, DATA_QUAT), // Geomagnetic Rotation Vector
	/* 042 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 043 */ FIFO_COMMAND(0, 1, DATA_EULR), // Orientation
	/* 044 */ FIFO_COMMAND(1, 0, DATA_EULR), // Orientation
	/* 045 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 046 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 047 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 048 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 049 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 050 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 051 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 052 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 053 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 054 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 055 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 056 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 057 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 058 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 059 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 060 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 061 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 062 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 063 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
};

uint8_t fifo_command_table3[] = {
	/* 096 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 097 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 098 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 099 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 100 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 101 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 102 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 103 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 104 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 105 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 106 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 107 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 108 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 109 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 110 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 111 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 112 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 113 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 114 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 115 */ FIFO_COMMAND(0, 1, DATA_AIRQ), // Air Quality
	/* 116 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 117 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 118 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 119 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 120 */ FIFO_COMMAND(0, 1, DATA_QUAT), // Head Orientation Misalignment
	/* 121 */ FIFO_COMMAND(0, 1, DATA_QUAT), // Head Orientation
	/* 122 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 123 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 124 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 125 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 126 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 127 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
};

uint8_t fifo_command_table4[] = {
	/* 128 */ FIFO_COMMAND(0, 1, DATA_UINT16),	 // Temperature
	/* 129 */ FIFO_COMMAND(0, 1, DATA_UINT24),	 // Barometer
	/* 130 */ FIFO_COMMAND(0, 1, DATA_UINT8),	 // Humidity
	/* 131 */ FIFO_COMMAND(0, 1, DATA_UINT32),	 // Gas
	/* 132 */ FIFO_COMMAND(1, 0, DATA_UINT16),	 // Temperature
	/* 133 */ FIFO_COMMAND(1, 0, DATA_UINT24),	 // Barometer
	/* 134 */ FIFO_COMMAND(1, 0, DATA_UINT8),	 // Humidity
	/* 135 */ FIFO_COMMAND(1, 0, DATA_UINT32),	 // Gas
	/* 136 */ FIFO_COMMAND(0, 1, DATA_UINT32),	 // Step Counter
	/* 137 */ FIFO_COMMAND(0, 1, DATA_EVT_NONE), // Step Detector
	/* 138 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 139 */ FIFO_COMMAND(1, 0, DATA_UINT32),	 // Step Counter
	/* 140 */ FIFO_COMMAND(1, 0, DATA_EVT_NONE), // Step Detector
	/* 141 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 142 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 143 */ FIFO_COMMAND(1, 0, DATA_EVT_NONE), // Any Motion
	/* 144 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 145 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 146 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 147 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 148 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 149 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 150 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 151 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 152 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 153 */ FIFO_COMMAND(0, 1, DATA_EVT_TAP),	 // Multi-Tap Detector
	/* 154 */ FIFO_COMMAND(1, 0, DATA_EVT_ACT),	 // Activity Recognition
	/* 155 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 156 */ FIFO_COMMAND(1, 0, DATA_EVT_WST),	 // Wrist gesture
	/* 157 */ FIFO_COMMAND(0, 0, DATA_NONE),	 // Undefined
	/* 158 */ FIFO_COMMAND(1, 0, DATA_EVT_WST),	 // Wrist wear
	/* 159 */ FIFO_COMMAND(1, 0, DATA_EVT_NONE), // No Motion
};

uint8_t fifo_command_table7[] = {
	/* 224 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 225 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 226 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 227 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 228 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 229 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 230 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 231 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 232 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 233 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 234 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 235 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 236 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 237 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 238 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 239 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 240 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 241 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 242 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 243 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 244 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 245 */ FIFO_COMMAND(1, 0, DATA_TIME), // Timestamp Small Delta
	/* 246 */ FIFO_COMMAND(1, 0, DATA_TIME), // Timestamp Large Delta
	/* 247 */ FIFO_COMMAND(1, 0, DATA_TIME), // Timestamp Full
	/* 248 */ FIFO_COMMAND(1, 0, DATA_META), // Metadata
	/* 249 */ FIFO_COMMAND(0, 0, DATA_NONE), // Undefined
	/* 250 */ FIFO_COMMAND(0, 1, DATA_DEBG), // Debug Data
	/* 251 */ FIFO_COMMAND(0, 1, DATA_TIME), // Timestamp Small Delta
	/* 252 */ FIFO_COMMAND(0, 1, DATA_TIME), // Timestamp Large Delta
	/* 253 */ FIFO_COMMAND(0, 1, DATA_TIME), // Timestamp Full
	/* 254 */ FIFO_COMMAND(0, 1, DATA_META), // Metadata
	/* 255 */ FIFO_COMMAND(1, 1, DATA_NONE), // Undefined
};