#include "modem.h"
#include "uart_switch.h"
#include "led.h"
#include <zephyr/kernel.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/sys/printk.h>

#define AT_LINE_MAX   256u
#define AT_TIMEOUT_MS 10000

static char m_line[AT_LINE_MAX];

static int read_line(int timeout_ms)
{
    int64_t deadline = k_uptime_get() + timeout_ms;
    uint8_t byte;
    int     len = 0;

    while (k_uptime_get() < deadline) {
        if (!uart_read_byte(&byte, K_MSEC(100))) {
            continue;
        }
        if (byte == '\r') {
            continue;
        }
        if (byte == '\n') {
            m_line[len] = '\0';
            return len;
        }
        if (len < (int)(AT_LINE_MAX - 1)) {
            m_line[len++] = (char)byte;
        }
    }
    m_line[len] = '\0';
    return -1;
}

static bool at_send(const char *cmd, const char *expect, int timeout_ms)
{
    printk("AT >> %s\n", cmd);
    uart_write_str(cmd);
    uart_write_str("\r\n");

    int64_t deadline = k_uptime_get() + timeout_ms;
    while (k_uptime_get() < deadline) {
        int remaining = (int)(deadline - k_uptime_get());
        if (remaining <= 0) {
            break;
        }
        int n = read_line(remaining < 500 ? remaining : 500);
        if (n < 0) {
            break;
        }
        if (n == 0) {
            continue; 
        }
        printk("AT << %s\n", m_line);
        if (strstr(m_line, expect)) {
            return true;
        }
        if (strstr(m_line, "ERROR")) {
            return false;
        }
    }
    printk("AT timeout waiting for '%s'\n", expect);
    return false;
}

void modem_init(void)
{
    uart_switch_to_modem();
    uart_flush_rx();

    k_sleep(K_MSEC(2000));

    at_send("AT",          "OK",  AT_TIMEOUT_MS);
    at_send("ATE0",        "OK",  AT_TIMEOUT_MS);
    at_send("AT+CMEE=2",   "OK",  AT_TIMEOUT_MS);

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+CGDCONT=1,\"IP\",\"%s\"", MODEM_APN);
    at_send(cmd, "OK", AT_TIMEOUT_MS);

    printk("Modem init done\n");
}

void modem_sleep(void)
{
    uart_switch_to_modem();
    at_send("AT+CSCLK=1", "OK", AT_TIMEOUT_MS);
}

void modem_wake(void)
{
    uart_switch_to_modem();
    uart_write_str("AT\r\n");
    k_sleep(K_MSEC(300));
    at_send("AT+CSCLK=0", "OK", AT_TIMEOUT_MS);
}

bool modem_send_location(double lat, double lon)
{
    uart_switch_to_modem();
    uart_flush_rx();

    if (!at_send("AT+CREG?", "+CREG:", 15000)) {
        printk("Modem: not registered\n");
        return false;
    }
    if (!strstr(m_line, ",1") && !strstr(m_line, ",5")) {
        printk("Modem: registration status not OK: %s\n", m_line);
        return false;
    }

    at_send("AT+CGACT=1,1", "OK", 30000);

    int lat_sign = (lat < 0.0) ? -1 : 1;
    double lat_a = (lat < 0.0) ? -lat : lat;
    int lon_sign = (lon < 0.0) ? -1 : 1;
    double lon_a = (lon < 0.0) ? -lon : lon;

    int lat_deg  = (int)lat_a;
    int lat_frac = (int)((lat_a - lat_deg) * 1000000.0 + 0.5);
    int lon_deg  = (int)lon_a;
    int lon_frac = (int)((lon_a - lon_deg) * 1000000.0 + 0.5);

    char url[512];
    snprintf(url, sizeof(url),
             "AT+HTTPPARA=\"URL\",\"%s?lat=%s%d.%06d&lon=%s%d.%06d\"",
             TRACKER_ENDPOINT,
             (lat_sign < 0) ? "-" : "", lat_deg, lat_frac,
             (lon_sign < 0) ? "-" : "", lon_deg, lon_frac);

    if (!at_send("AT+HTTPINIT", "OK", 5000)) {
        return false;
    }
    if (!at_send("AT+HTTPPARA=\"CID\",1", "OK", 5000)) {
        at_send("AT+HTTPTERM", "OK", 5000);
        return false;
    }
    if (!at_send(url, "OK", 5000)) {
        at_send("AT+HTTPTERM", "OK", 5000);
        return false;
    }

    led_tx_on();

    bool ok = false;
    if (at_send("AT+HTTPACTION=0", "+HTTPACTION:", 30000)) {
        char *p = strstr(m_line, ",");
        if (p) {
            int status_code = atoi(p + 1);
            printk("HTTP status: %d\n", status_code);
            ok = (status_code == 200);
        }
    }

    led_tx_off();

    at_send("AT+HTTPTERM", "OK", 5000);

    return ok;
}
