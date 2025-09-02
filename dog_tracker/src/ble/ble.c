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

/* 修复：使用默认日志级别配置 */
LOG_MODULE_REGISTER(ble, CONFIG_LOG_DEFAULT_LEVEL);

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

    if (current_conn == conn) {
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
        LOG_INF("Disconnected: %s (reason 0x%02x)", addr, reason);
        
        bt_conn_unref(current_conn);
        current_conn = NULL;

        /* 如果之前延迟停止广播，现在停止 */
        if (defer_ble_stop) {
            LOG_INF("Stopping BLE advertising after deferred stop");
            bt_le_adv_stop();
            defer_ble_stop = false;
        }
    }
}

/* 连接回调结构 */
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

/* 安全相关回调 */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Passkey for %s: %06u", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Pairing cancelled: %s", addr);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_WRN("Pairing failed conn: %s, reason %d", addr, reason);
    
    /* 断开连接 */
    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

/* 认证回调结构 */
static struct bt_conn_auth_cb conn_auth_callbacks = {
    .passkey_display = auth_passkey_display,
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
    
    if (serial_num[UICR_STR_MAX_LEN - 2] == '\0') {
        /* 完整序列号 */
        snprintf(local_name_str, sizeof(local_name_str), "Tracker-%s", serial_num);
    } else {
        /* 截断的序列号，显示前10个字符 */
        snprintf(local_name_str, sizeof(local_name_str), "Tracker-%.10s", serial_num);
    }

    LOG_INF("Device name: %s", local_name_str);

    /* 注册认证回调 */
    err = bt_conn_auth_cb_register(&conn_auth_callbacks);
    if (err) {
        LOG_ERR("Failed to register auth callbacks (err %d)", err);
        return err;
    }

    err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
    if (err) {
        LOG_ERR("Failed to register auth info callbacks (err %d)", err);
        return err;
    }

    /* 初始化Tracker服务 */
    err = tracker_service_init();
    if (err) {
        LOG_ERR("Failed to initialize tracker service (err %d)", err);
        return err;
    }

    LOG_INF("Bluetooth initialization complete");
    return 0;
}