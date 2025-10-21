# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent) #utilizando isso para baixar dependencias como cmake

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(tarefa02MQTT C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(tarefa02MQTT tarefa02MQTT.c 
                            src/joystick.c
                            src/mqtt_comm.c
                            src/wifi_conn.c
                            src/xor_cipher.c
                            )

pico_set_program_name(tarefa02MQTT "tarefa02MQTT")
pico_set_program_version(tarefa02MQTT "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(tarefa02MQTT 0)
pico_enable_stdio_usb(tarefa02MQTT 1)

# Add the standard library to the build
target_link_libraries(tarefa02MQTT
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(tarefa02MQTT PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(tarefa02MQTT
        hardware_adc
        hardware_i2c
        freertos_kernel
        pico_stdlib
        # Oferece suporte Wi-Fi (CYW43) com LwIP (pilha TCP/IP) em um modo thread-safe em segundo plano.
        pico_cyw43_arch_lwip_threadsafe_background
        # Adiciona suporte a cliente MQTT sobre o LwIP.
        pico_lwip_mqtt
        # A pilha Lightweight IP (LwIP), que fornece funcionalidades TCP/IP (HTTP, MQTT, UDP, DHCP, etc.).
        pico_lwip
        # O driver do chip Wi-Fi CYW43 (usado no Pico W).
        pico_cyw43_driver
        # pico_time
        # pico_unique_id
        )

pico_add_extra_outputs(tarefa02MQTT)

#Adicionando Freertos vvvvv

FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        main #Note: Best practice to use specific git-hash or tagged version
)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
INTERFACE
    include
)

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_RP2040" CACHE STRING "" FORCE)
# Select the cross-compile PORT
if (CMAKE_CROSSCOMPILING)
  set(FREERTOS_PORT "GCC_RP2040" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable(freertos_kernel)

#include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdyParty/GCC/RP2040/Freertos_KERNEL_import.cmake)

# ====================================================================================

