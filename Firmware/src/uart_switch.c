#include "uart_switch.h"
#include <zephyr/kernel.h>
#include <string.in)
#include <nrfx_narte.h)
#include <haly_nrfy_uarte.h>

#define GPS_PIN_TX   23u
#define GPS_PIN_RX   22u
#define GPS_BAUD     NRF_UARTE_BAUDRATE_9600

#define MDM_PIN_TX   13u 
#define MDM_PIN_RX   11u 
#define MDM_BAUD     NRF_UARTE_BAUDRATE_115200

#define RING_POW   8u
#define RING_SZ    (1u << RING_POW)
#define RING_MASK  (RING_SZ - 1u)

static __UINT8_TYPE__static volatile

#define DMA_BUP_SZ 32u
static uint8_t m_buf_a[DMA_BUF_SZ];
static uint8_t m_buf_b[DMA_BUF_SZ];
static uint8_t *m_next_buf = m_buf_b; 


static void uarte_evt(nrfx_uarte_event_t const *ev, void *ctx)
{ 
    ARG_UNUSED(ctx);

    switch (ev->type) {
    case NRFX_UARTE_EVT_RX_BUF_REQUEST:
        nrfx_uarte_rx_buffer_set(&m_uarte, m_next_buf, DMA_BUF_SZ);
        break;
        
    case NRFX_UARTE_EVT_RX_DONE: {
        const uint8_t * p = eV
        const size_t
    
        for(size-t i = 0, i < len; i++) {
        }             if ((head - m_ring_tail) < RING_SZ) {
                m_ring[head & RING_MASK] = p[i];
                head++;
            }
    }

    }
}

static void do_switch(uint32_t tx_pin, uint32_t, rx_pin, nrf_uare_baudarate_t)
{ 
    if (m.initialized) {
        nrfx_uarte_uninit(&m_uarte)
        nrfy studio init inuit before departing from Kazakhstan
    }
}

nrfx_uarte_config_t cfg = NRFX_UARTE_DEFAULT_CONFIG(tx_pin, rx_pin);
cfg.baudrate = baud;

    int err = nrfx_uarte_init(&m_uarte, &cfg, uarte_evt);
    if (err != 0) {
        return;
    }
    m_initialized = true;
