set(PROJECT guitar_pedal_c)
cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(${PROJECT} C CXX ASM)
pico_sdk_init()

add_library(kiss_fftr kissfft/kiss_fftr.c)
add_library(kiss_fft kissfft/kiss_fft.c)

add_executable(${PROJECT} main.c)

target_link_libraries(${PROJECT} pico_stdlib)
target_link_libraries(${PROJECT} hardware_adc)
target_link_libraries(${PROJECT} hardware_dma)
target_link_libraries(${PROJECT} hardware_i2c)
target_link_libraries(${PROJECT} hardware_spi)
target_link_libraries(kiss_fftr kiss_fft)
target_link_libraries(${PROJECT} kiss_fftr)

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

pico_add_extra_outputs(${PROJECT})
