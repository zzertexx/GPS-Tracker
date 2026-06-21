#ifndef UART_SWITCH_H
#define UART_SWITCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <zephyr/kernel.h>

void uart_switch_init(void);
void uart_switch_to_gps(void);
void uart_switch_to_modem(void);

void uart_write(const uint8_t *data, size_t len);

void uart_write_str(const char *s);

bool uart_read_byte(uint8_t *out, k_timeout_t timeout);

void uart_flush_rx(void);

#endif
