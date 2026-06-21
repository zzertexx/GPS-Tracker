#include "uart_switch.h"
#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <string.h>
#include <nrfx_uarte.h>       
#include <haly/nrfy_uarte.h>

/* ── Pin assignments ─────────────────────────────────────────────────── */
#define GPS_PIN_TX   23u 
#define GPS_PIN_RX   22u  
#define GPS_BAUD     NRF_UARTE_BAUDRATE_9600

#define MDM_PIN_TX   13u   
#define MDM_PIN_RX   11u   
#define MDM_BAUD     NRF_UARTE_BAUDRATE_115200

/* ── RX ring buffer ──────────────────────────────────────────────────── */
#define RING_POW   8u                
#define RING_SZ    (1u << RING_POW)
#define RING_MASK  (RING_SZ - 1u)

static uint8_t         m_ring[RING_SZ];
static volatile uint32_t m_ring_head; 
static uint32_t        m_ring_tail;   

/* ── DMA ping-pong buffers ───────────────────────────────────────────── */
#define DMA_BUF_SZ  32u
static uint8_t m_buf_a[DMA_BUF_SZ];
static uint8_t m_buf_b[DMA_BUF_SZ];
static uint8_t *m_next_buf = m_buf_b;  

/* ── nrfx instance ───────────────────────────────────────────────────── */
static nrfx_uarte_t m_uarte = NRFX_UARTE_INSTANCE(0);
static bool         m_initialized;

/* ── ISR / event handler ─────────────────────────────────────────────── */
static void uarte_evt(nrfx_uarte_event_t const *ev, void *ctx)
{
    ARG_UNUSED(ctx);

    switch (ev->type) {
    case NRFX_UARTE_EVT_RX_BUF_REQUEST:
        nrfx_uarte_rx_buffer_set(&m_uarte, m_next_buf, DMA_BUF_SZ);
        break;

    case NRFX_UARTE_EVT_RX_DONE: {
        const uint8_t *p   = ev->data.rx.p_data;
        size_t         len = ev->data.rx.length;
        uint32_t       head = m_ring_head;

        for (size_t i = 0; i < len; i++) {
            if ((head - m_ring_tail) < RING_SZ) {
                m_ring[head & RING_MASK] = p[i];
                head++;
            }
        }
        m_ring_head = head;

        m_next_buf = (uint8_t *)p;
        break;
    }

    case NRFX_UARTE_EVT_TX_DONE:
        break;

    case NRFX_UARTE_EVT_ERROR:
        break;

    default:
        break;
    }
}

static void do_switch(uint32_t tx_pin, uint32_t rx_pin, nrf_uarte_baudrate_t baud)
{
    if (m_initialized) {
        nrfx_uarte_uninit(&m_uarte);
        nrfy_uarte_int_init(NRF_UARTE0, 0xFFFFFFFFu, 0, false);
        m_initialized = false;
    }

    nrfx_uarte_config_t cfg = NRFX_UARTE_DEFAULT_CONFIG(tx_pin, rx_pin);
    cfg.baudrate = baud;

    int err = nrfx_uarte_init(&m_uarte, &cfg, uarte_evt);
    if (err != 0) {
        return;
    }
    m_initialized = true;

    m_ring_head = 0;
    m_ring_tail = 0;
    m_next_buf  = m_buf_b;

    nrfx_uarte_rx_buffer_set(&m_uarte, m_buf_a, DMA_BUF_SZ);
    nrfx_uarte_rx_enable(&m_uarte, 0);
}

/* ── Public API ──────────────────────────────────────────────────────── */

void uart_switch_init(void)
{
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(uart0)),
                DT_IRQ(DT_NODELABEL(uart0), priority),
                nrfx_isr,
                nrfx_uarte_0_irq_handler,
                0);
    irq_enable(DT_IRQN(DT_NODELABEL(uart0)));

    do_switch(GPS_PIN_TX, GPS_PIN_RX, GPS_BAUD);
}

void uart_switch_to_gps(void)
{
    do_switch(GPS_PIN_TX, GPS_PIN_RX, GPS_BAUD);
}

void uart_switch_to_modem(void)
{
    do_switch(MDM_PIN_TX, MDM_PIN_RX, MDM_BAUD);
}

void uart_write(const uint8_t *data, size_t len)
{
    if (!m_initialized || len == 0) {
        return;
    }
    nrfx_uarte_tx(&m_uarte, data, len, 0);
}

void uart_write_str(const char *s)
{
    if (s) {
        uart_write((const uint8_t *)s, strlen(s));
    }
}

bool uart_read_byte(uint8_t *out, k_timeout_t timeout)
{
    int64_t deadline = k_uptime_get() + k_ticks_to_ms_floor64(timeout.ticks);

    while (m_ring_tail == m_ring_head) {
        if (k_uptime_get() >= deadline) {
            return false;
        }
        k_sleep(K_MSEC(1));
    }

    *out = m_ring[m_ring_tail & RING_MASK];
    m_ring_tail++;
    return true;
}

void uart_flush_rx(void)
{
    m_ring_tail = m_ring_head;
}
