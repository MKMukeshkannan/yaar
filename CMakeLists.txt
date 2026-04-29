cmake_minimum_required(VERSION 3.30)
include(pico_sdk_import.cmake)


project(yaar C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLE_PATH ${PROJECT_SOURCE_DIR})

pico_sdk_init()

add_executable(${PROJECT_NAME} 
    ./src/main.c
    ./src/display_config.c
    ./src/sd_config.c
    ./src/yaar_state.c
)
add_subdirectory(vendor)
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c FatFs_SPI)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
