cmake_minimum_required(VERSION 3.25)

#
# Set PICO_SDK_PATH
#
if (NOT PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message("Setting PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
    else()
        set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/pico-sdk)
        message("Setting PICO_SDK_PATH from git submodule path ('${PICO_SDK_PATH}')")
    endif()
else()
    message("PICO_SDK_PATH already set ('${PICO_SDK_PATH}')")
endif()

#
# Set PICO_TINYUSB_PATH
#
if (NOT PICO_TINYUSB_PATH)
    if(DEFINED ENV{PICO_TINYUSB_PATH})
        set(PICO_TINYUSB_PATH $ENV{PICO_TINYUSB_PATH})
        message("Setting PICO_TINYUSB_PATH from environment ('${PICO_TINYUSB_PATH}')")
    else()
        set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/tinyusb)
        message("Setting PICO_TINYUSB_PATH from git submodule path ('${PICO_TINYUSB_PATH}')")
    endif()
else()
    message("PICO_TINYUSB_PATH already set ('${PICO_TINYUSB_PATH}')")
endif()

#
# Extract Dependencies for PICO_TINYUSB_PATH
# This enables Pico-PIO-USB
#
find_package(Python3 COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} ${PICO_TINYUSB_PATH}/tools/get_deps.py rp2040)

#
# Pico SDK Setup
#
set(PICO_BOARD pico_w)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# 
# Generic Setup
#
set(FAMILY rp2040)
set(PROJECT gemini-i)
project(${PROJECT} C CXX ASM)
add_executable(${PROJECT})

#
# TinyUSB setup
#
include(${PICO_TINYUSB_PATH}/hw/bsp/family_support.cmake)
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})
# family_add_pico_pio_usb(${PROJECT})


#
# Configure Soruces
#
file(GLOB_RECURSE FILES src/*.c src/*.cpp src/*.h src/*.hpp src/*.pio)
target_sources(${PROJECT} PUBLIC ${FILES})
target_include_directories(${PROJECT} PUBLIC 
    ${PICO_TINYUSB_PATH}/src
    ${PICO_TINYUSB_PATH}/hw
	${CMAKE_CURRENT_LIST_DIR}/src
	${CMAKE_CURRENT_LIST_DIR}/firmware
)

target_link_libraries(${PROJECT} PRIVATE 
    pico_stdlib
    pico_cyw43_arch_none
    pico_bootrom
    pico_malloc
    pico_runtime
    pico_multicore
    pico_sync
    pico_rand
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_spi
    hardware_clocks
    hardware_watchdog
    hardware_pio
    hardware_dma
    hardware_uart
    hardware_flash
    tinyusb_device
    tinyusb_host
    tinyusb_board
    tinyusb_pico_pio_usb
)
target_link_options(${PROJECT} PRIVATE -Xlinker --print-memory-usage)

target_compile_definitions(${PROJECT} PRIVATE
    PICO_PANIC_FUNCTION=tusb_panic
)

#
# Miscelaneous Setup
#
pico_enable_stdio_uart(${PROJECT} 0)
pico_add_extra_outputs(${PROJECT})

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_LIST_DIR}/dist
)
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_LIST_DIR}/build/${PROJECT}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/dist/${PROJECT}.uf2
)