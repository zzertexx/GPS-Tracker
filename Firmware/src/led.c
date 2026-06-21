#include "led.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec m_led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

void led_init(void)
{
    gpio_pin_configure_dt(&m_led, GPIO_OUTPUT_INACTIVE);
}

void led_tx_on(void)
{
    gpio_pin_set_dt(&m_led, 1);
}

void led_tx_off(void)
{
    gpio_pin_set_dt(&m_led, 0);
}

void led_blink_fail(void)
{
    for (int i = 0; i < 3; i++) {
        gpio_pin_set_dt(&m_led, 1);
        k_sleep(K_MSEC(80));
        gpio_pin_set_dt(&m_led, 0);
        k_sleep(K_MSEC(120));
    }
}
