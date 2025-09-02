/* src/ble/ble.c */
/*
 * Copyright (c) 2024 Dog Tracker Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/types.h>
#include <stdio.h>

#include "ble.h"
#include "tracker_service.h"
#include "uicr.h"

LOG_MODULE_REGISTER(ble, CONFIG_BLE_LOG_LEVEL);

/* BLE连接状态 */
static struct bt_conn *current_conn;
static struct bt_conn *auth_conn;

/* 公司ID - 可根据实际情况修改 */
static uint16_t company_id = 0x0059; /* Nordic Semiconductor */

/* 设备名称缓冲区 */
static char local_name_str[25]; /* 确保不超过广播数据限制 */

/* BLE广播超时配置 */
#define BLE_ADV_TIME 600 /* 默认10分钟广播超时 */
static bool defer_ble_stop = false;

/* BLE广播超时处理 */
void ble_adv_timeout_worker(struct k_work *work)
{
    LOG_INF("BLE advertising timeout");
    
    if (current_conn == NULL) {
        bt_le_adv_stop();
        LOG_INF("BLE advertising stopped due to timeout");
        defer_ble_stop = false;
    } else {
        LOG_INF("Deferring BLE stop - connection active");
        defer_ble_stop = true;
    }
}

K_WORK_DEFINE(ble_adv_work, ble_adv_timeout_worker);

void ble_adv_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&ble_adv_work);
}

K_TIMER_DEFINE(ble_adv_timer, ble_adv_timer_handler, NULL);

/* 获取当前连接 */
struct bt_conn *ble_get_conn(void)
{
    return current_conn;
}

/* 连接建立回调 */
static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected to %s", addr);

    if (current_conn == NULL) {
        current_conn = bt_conn_ref(conn);
    } else {
        LOG_WRN("Multiple connections not supported");
    }
}

/* 连接断开回调 */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (auth_conn) {
        bt_conn_unref(auth_conn);
        auth_conn = NULL;
    }

    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Disconnected from %s (reason %u)", addr, reason);

    /* 处理延迟的广播停止 */
    if (defer_ble_stop) {
        LOG_INF("Executing deferred BLE stop");
        bt_le_adv_stop();
        defer_ble_stop = false;
    }
}

/* MTU更新回调 */
void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
    LOG_INF("MTU updated: TX: %d RX: %d bytes", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = { 
    .att_mtu_updated = mtu_updated 
};

/* 安全等级变更回调 */
static void security_changed(struct bt_conn *conn, bt_security_t level, 
                           enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_INF("Security changed: %s level %u", addr, level);
        
        if (bt_conn_get_security(conn) >= BT_SECURITY_L2) {
            LOG_INF("Security level >= L2 achieved");
        }
    } else {
        LOG_ERR("Security failed: %s level %u err %d", addr, level, err);
    }
}

/* 连接回调结构 */
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected        = connected,
    .disconnected     = disconnected,
    .security_changed = security_changed,
};

/* 认证取消回调 */
static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_WRN("Pairing cancelled: %s", addr);

    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

/* 配对完成回调 */
static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
}

/* 配对失败回调 */
static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_ERR("Pairing failed: %s, reason %d", addr, reason);

    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

/* 认证回调结构 */
static struct bt_conn_auth_cb conn_auth_callbacks = {
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = { 
    .pairing_complete = pairing_complete,
    .pairing_failed   = pairing_failed 
};

/* 开始BLE广播 */
int ble_advertise_start(int timeout_sec)
{
    int err = 0;

    /* 广播数据 */
    const struct bt_data advertisement_data[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA(BT_DATA_NAME_COMPLETE, local_name_str, strlen(local_name_str)),
    };

    /* 制造商数据：公司ID + 序列号 */
    uint8_t mfg_data[sizeof(company_id) + UICR_STR_MAX_LEN - 1];
    memcpy(mfg_data, &company_id, sizeof(company_id));

    char *serial_num = uicr_serial_number_get();
    memcpy(&mfg_data[sizeof(company_id)], serial_num, UICR_STR_MAX_LEN - 1);

    /* 扫描响应数据 */
    const struct bt_data scan_response_data[] = { 
        BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)) 
    };

    err = bt_le_adv_start(
        BT_LE_ADV_CONN,
        advertisement_data,
        ARRAY_SIZE(advertisement_data),
        scan_response_data,
        ARRAY_SIZE(scan_response_data));
        
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }
    
    LOG_INF("BLE advertising started for %d seconds", timeout_sec);
    
    if (timeout_sec > 0) {
        k_timer_start(&ble_adv_timer, K_SECONDS(timeout_sec), K_NO_WAIT);
    }
    
    return 0;
}

/* 停止BLE广播 */
int ble_stop(void)
{
    if (current_conn == NULL) {
        LOG_INF("Stopping BLE advertising");
        bt_le_adv_stop();
    } else {
        LOG_INF("Connection active, deferring BLE stop");
        defer_ble_stop = true;
    }
    return 0;
}

/* BLE初始化 */
int ble_init(void)
{
    int err = 0;

    LOG_INF("Initializing Bluetooth...");

    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return err;
    }

    /* 加载设置 */
    settings_load();

    /* 生成设备名称 */
    char *serial_num = uicr_serial_number_get();
    
    if (serial_num[UICR_STR_MAX_LEN - 7] != 0xFF) {
        /* 使用序列号的最后6位 */
        snprintf(
            local_name_str,
            sizeof(local_name_str),
            "Dog Tracker %c%c%c%c%c%c",
            serial_num[UICR_STR_MAX_LEN - 7],
            serial_num[UICR_STR_MAX_LEN - 6],
            serial_num[UICR_STR_MAX_LEN - 5],
            serial_num[UICR_STR_MAX_LEN - 4],
            serial_num[UICR_STR_MAX_LEN - 3],
            serial_num[UICR_STR_MAX_LEN - 2]);
    } else {
        /* 使用蓝牙MAC地址 */
        bt_addr_le_t addr = { 0 };
        size_t count = 1;
        bt_id_get(&addr, &count);
        
        snprintf(local_name_str, sizeof(local_name_str), 
                "Dog Tracker %02X%02X%02X", 
                addr.a.val[2], addr.a.val[1], addr.a.val[0]);
    }
    
    LOG_INF("BLE Device Name: %s", local_name_str);

    /* 设置GAP设备名称 */
    bt_set_name(local_name_str);

    /* 注册GATT回调 */
    bt_gatt_cb_register(&gatt_callbacks);

    /* 注册认证回调 */
    err = bt_conn_auth_cb_register(&conn_auth_callbacks);
    if (err) {
        LOG_ERR("Failed to register authorization callbacks: %d", err);
        return err;
    }

    err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
    if (err) {
        LOG_ERR("Failed to register authorization info callbacks: %d", err);
        return err;
    }

    /* 初始化Tracker服务 */
    tracker_service_init();
    
    LOG_INF("Bluetooth initialized successfully");
    
    /* 如果是出厂设备，自动开始广播 */
    if (uicr_shipping_flag_get()) {
        LOG_INF("Shipping mode - starting BLE advertising");
        return ble_advertise_start(BLE_ADV_TIME);
    }
    
    return 0;
}

/* 强制断开连接 */
int ble_disconnect(void)
{
    if (current_conn) {
        LOG_INF("Forcing BLE disconnect");
        return bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    return 0;
}

/* 关闭BLE功能 */
int ble_shutdown(void)
{
    LOG_INF("Shutting down BLE");
    bt_le_adv_stop();
    
    if (current_conn) {
        bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_POWER_OFF);
    }
    
    return 0;
}

/* 获取本地设备名称 */
char *ble_get_local_name(void)
{
    return local_name_str;
}