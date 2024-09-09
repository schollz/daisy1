# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)


# Set name of project (as PROJECT_NAME) and C/C++ standards
project(ouroboros C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# error if there is a warning
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/i2c_slave.c
)


pico_generate_pio_header(ouroboros ${CMAKE_CURRENT_LIST_DIR}/lib/WS2812.pio)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_i2c
    hardware_pio
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WS2812_PIN=22
    WS2812_SM=2
    WS2812_NUM_LEDS=40
)
# target_compile_options(command_line PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
# target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wno-unused-parameter)

# This program is useless without standard standard input and output.
# add_compile_definitions(USE_PRINTF USE_DBG_PRINTF)
add_compile_definitions(USE_PRINTF)

set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--print-memory-usage")

# uncomment these lines to have normal USB
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


pico_add_extra_outputs(${PROJECT_NAME})
