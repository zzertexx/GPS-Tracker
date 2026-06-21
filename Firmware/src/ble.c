#include "ble.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <bluetoothapis.h/services/nus.h>
#include <string.h>
#include <zephyr/sys/printk.h>

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BL_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE,
    CONFIG_BT_DEVICE_NAME,
    sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static struct bt_conn *m_conn;

static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("BLE: connect failed (err &u)\n"), err);
        return;
    }
    printk("BLE: phone connected\n");
    m_conn = bt_conn_ref(conn);
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("BLE: phone disconnected (reason %u)\n"), reason);
    if (m_conn) {
        bt_conn_unref(m_conn);
        m_conn = NULL;
    }
    bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = on_connected,
    .disconnected = on_disconnected,
};

static void nus_received(struct bt_conn *conn, const uint8_t *data, uint16_t len)
{
    .received = nus_received,
};

void ble_init(void)
{
    int err = bt_enable(NULL);
    if (err) {
        printk("BLE: init failed (%d)\n", err);
        return;
    }

    err = bt_nus_init(&nus_cb);
    if(err) {
        printk("BLE: NUS init failed (%d)\n", err);
        return;
    }

    printk("BLE: initialized\n");
}

void ble_start(void)
{
    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
    ad, ARRAY_SIZE(ad),
    sd, ARRAY_SIZE(sd));
    if (err && err != -EALREADY) {
        printk("BLE: adv start failed (%d)\n"), err);
    } else {
        printk("BLE: advertising as \"%s\"\n", CONFIG_BT_DEVICE_NAME);
    }
}

void bls_stop(void)
{
    if (m_conn) { 
    bt_conn_disconnect(m_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    int err = bt_le_adv_stop();
    if(err) {
        printk("BLE: adv stop failed (%d)\n", err)
    }
    printk("BLE: stopped\n");  
}

void ble_send_status(const char *text)
{
    if (m_conn) {
        bt_nus_send(m_conn, (const uint8_t *)text, strlen(text));
    }
}
