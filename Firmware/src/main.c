#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "uart_switch.h"
#include "led.h"
#include "gps.h"
#include "modem.h"
#include "ble.h"
#include "state_machine.h"

int main(void)
{
    printk("AKT-01 tracker boot\n");

    uart_switch_init();
    led_init();
    gps_init();
    ble_init();
    modem_init();
    state_machine_init();

    printk("Boot complete\n");

    state_machine_run();

    return 0;
}
