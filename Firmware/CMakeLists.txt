# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky_1)

target_sources(app PRIVATE
    src/main.c
    src/uart_switch.c
    src/led.c
    src/gps.c
    src/modem.c
    src/ble.c
    src/state_machine.c
)
