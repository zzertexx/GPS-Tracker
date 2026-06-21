#include "state_machine.h"
#include "gps.h"
#include "modem.h"
#include "ble.h"
#include "led.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* ── State ───────────────────────────────────────────────────────────── */
typedef enum { STATE_SLEEP, STATE_AWAKE } device_state_t;
static volatile device_state_t m_state = STATE_SLEEP;

/* ── Button ──────────────────────────────────────────────────────────── */
#define BUTTON_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec m_btn = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback      m_btn_cb;

static K_SEM_DEFINE(m_btn_sem, 0, 1);

static void button_isr(const struct device *dev, struct gpio_callback *cb,
                        uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);
    k_sem_give(&m_btn_sem);
}

/* ── Cycle timer + semaphore ─────────────────────────────────────────── */
static K_SEM_DEFINE(m_cycle_sem, 0, 1);

static void cycle_timer_cb(struct k_timer *t)
{
    ARG_UNUSED(t);
    k_sem_give(&m_cycle_sem);
}

K_TIMER_DEFINE(m_cycle_timer, cycle_timer_cb, NULL);

/* ── Cycle thread ────────────────────────────────────────────────────── */
#define CYCLE_THREAD_STACK  4096
#define CYCLE_THREAD_PRIO   5

static void cycle_thread_fn(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    while (1) {
        k_sem_take(&m_cycle_sem, K_FOREVER);

        if (m_state != STATE_AWAKE) {
            continue;
        }

        printk("Cycle: starting fix→upload\n");

        /* ── GPS ─────────────────────────────────────────────────────── */
        gps_fix_t fix;
        bool got_fix = gps_acquire_fix(&fix, GPS_FIX_TIMEOUT_MS);

        if (!got_fix) {
            printk("Cycle: no GPS fix — skipping upload\n");
            led_blink_fail();
            continue;
        }

        /* ── Upload ──────────────────────────────────────────────────── */
        bool uploaded = modem_send_location(fix.latitude, fix.longitude);

        if (!uploaded) {
            printk("Cycle: upload failed\n");
            led_blink_fail();
        } else {
            printk("Cycle: upload OK\n");

            char buf[64];
            int lat_deg  = (int)(fix.latitude < 0 ? -fix.latitude : fix.latitude);
            int lat_frac = (int)((fix.latitude < 0 ? -fix.latitude : fix.latitude) * 1e6) % 1000000;
            int lon_deg  = (int)(fix.longitude < 0 ? -fix.longitude : fix.longitude);
            int lon_frac = (int)((fix.longitude < 0 ? -fix.longitude : fix.longitude) * 1e6) % 1000000;

            snprintf(buf, sizeof(buf), "%s%d.%06d,%s%d.%06d",
                     fix.latitude  < 0 ? "-" : "", lat_deg, lat_frac,
                     fix.longitude < 0 ? "-" : "", lon_deg, lon_frac);
            ble_send_status(buf);
        }
    }
}

K_THREAD_DEFINE(cycle_tid,
                CYCLE_THREAD_STACK,
                cycle_thread_fn,
                NULL, NULL, NULL,
                CYCLE_THREAD_PRIO, 0, 0);

/* ── State transitions ───────────────────────────────────────────────── */
static void enter_awake(void)
{
    printk("-> AWAKE\n");
    m_state = STATE_AWAKE;
    ble_start();
    k_sem_give(&m_cycle_sem);
    k_timer_start(&m_cycle_timer,
                  K_SECONDS(CYCLE_INTERVAL_S),
                  K_SECONDS(CYCLE_INTERVAL_S));
}

static void enter_sleep(void)
{
    printk("-> SLEEP\n");
    m_state = STATE_SLEEP;
    k_timer_stop(&m_cycle_timer);
    ble_stop();
    gps_enter_standby();
    modem_sleep();
}

/* ── Public API ──────────────────────────────────────────────────────── */

void state_machine_init(void)
{
    gpio_pin_configure_dt(&m_btn, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&m_btn, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&m_btn_cb, button_isr, BIT(m_btn.pin));
    gpio_add_callback(m_btn.port, &m_btn_cb);

    printk("State machine ready, starting in SLEEP\n");
}

void state_machine_run(void)
{
    while (1) {
        k_sem_take(&m_btn_sem, K_FOREVER);
        k_sleep(K_MSEC(50)); /* debounce */

        while (k_sem_take(&m_btn_sem, K_NO_WAIT) == 0) {
        }

        if (m_state == STATE_SLEEP) {
            enter_awake();
        } else {
            enter_sleep();
        }
    }
}
