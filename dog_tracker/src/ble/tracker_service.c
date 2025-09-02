/* src/ble/tracker_service.c */
/*
 * Copyright (c) 2024 Dog Tracker Project  
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>  // 修复：更新头文件
#include <zephyr/settings/settings.h>

#include <zcbor_decode.h>
#include <zcbor_encode.h>
#include <zcbor_common.h>

#include "app_version.h"
#include "ble.h"
#include "tracker_service.h"
#include "uicr.h"

LOG_MODULE_REGISTER(tracker_service, CONFIG_LOG_DEFAULT_LEVEL);

#define ONBOARDED_SETTINGS_PATH "tracker/onboarded"

/* 定义UUID */
static struct bt_uuid_128 tracker_service_uuid = BT_UUID_INIT_128(BT_UUID_TRACKER_SERVICE_VAL);

/* 通知特征值 */
static struct bt_uuid_128 service_notify_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x9d1589a6, 0xcea6, 0x4df1, 0x96d9, 0x1697cd4dc1e8));

/* 设备信息特征值 */
static struct bt_uuid_128 device_info_rx_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x9d1589a6, 0xcea6, 0x4df1, 0x96d9, 0x1697cd4dc200));

static struct bt_uuid_128 set_device_info_tx_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x9d1589a6, 0xcea6, 0x4df1, 0x96d9, 0x1697cd4dc201));

/* Ping特征值 */
static struct bt_uuid_128 set_ping_tx_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x9d1589a6, 0xcea6, 0x4df1, 0x96d9, 0x1697cd4dc202));

/* 通知使能标志 */
static volatile bool notify_enable = false;

/* 响应缓冲区 */
#define MAX_RESPONSE_LEN (512)
typedef struct {
    uint8_t  response[MAX_RESPONSE_LEN];
    uint16_t response_len;
} read_response_t;

static read_response_t read_response;

/* 辅助函数声明 */
static int device_info_resp(void);
static int ping_notify(void);

/* 设置设备信息处理函数 */
static ssize_t set_device_info(
    struct bt_conn *conn, const struct bt_gatt_attr *attr, 
    const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    LOG_HEXDUMP_DBG(buf, len, "DEVICE_INFO");
    
    bool success;
    struct zcbor_string decoded_key;
    uint64_t ts;

    /* 修复：创建CBOR解码状态，添加第6个参数 */
    ZCBOR_STATE_D(decoding_state, 3, buf, len, 2, 0);

    /* 开始解码映射 */
    success = zcbor_map_start_decode(decoding_state);
    if (!success) {
        goto error;
    }

    success = zcbor_tstr_decode(decoding_state, &decoded_key);
    if (!success) {
        goto error;
    }

    LOG_DBG("Decoded key: '%.*s'", (int)decoded_key.len, decoded_key.value);

    if (strncmp(decoded_key.value, "UTC_TIME", strlen("UTC_TIME")) == 0) {
        /* 设置时间 */
        success = zcbor_uint64_decode(decoding_state, &ts);
        if (!success) {
            goto error;
        }
        LOG_DBG("Decoded time: %llu", ts);
        /* 这里可以调用时间设置函数 */
        
    } else if (strncmp(decoded_key.value, "CLEAR_BONDING", strlen("CLEAR_BONDING")) == 0) {
        bool clear_bonding;
        success = zcbor_bool_decode(decoding_state, &clear_bonding);
        if (!success) {
            goto error;
        }
        
        if (clear_bonding) {
            LOG_INF("Clearing BLE bonding info");
            int err = bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
            if (err) {
                LOG_ERR("Unpairing failed (err %d)", err);
            }
        }
        
    } else if (strncmp(decoded_key.value, "SET_ONBOARDED", strlen("SET_ONBOARDED")) == 0) {
        bool onboarded;
        success = zcbor_bool_decode(decoding_state, &onboarded);
        if (!success) {
            goto error;
        }
        
        LOG_INF("Setting onboarded flag to %d", onboarded);
        settings_save_one(ONBOARDED_SETTINGS_PATH, &onboarded, sizeof(onboarded));
    }

    /* 结束解码映射 */
    success = zcbor_map_end_decode(decoding_state);
    if (!success) {
        goto error;
    }

    return len;

error:
    LOG_ERR("CBOR decoding failed: %d", zcbor_peek_error(decoding_state));
    return -1;
}

/* Ping处理函数 */
static ssize_t set_ping(
    struct bt_conn *conn, const struct bt_gatt_attr *attr, 
    const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    LOG_HEXDUMP_DBG(buf, len, "PING");
    
    bool success;
    struct zcbor_string decoded_key;

    /* 修复：创建CBOR解码状态，添加第6个参数 */
    ZCBOR_STATE_D(decoding_state, 3, buf, len, 2, 0);

    /* 开始解码映射 */
    success = zcbor_map_start_decode(decoding_state);
    if (!success) {
        goto error;
    }

    success = zcbor_tstr_decode(decoding_state, &decoded_key);
    if (!success) {
        goto error;
    }

    LOG_DBG("Decoded key: '%.*s'", (int)decoded_key.len, decoded_key.value);

    if (strncmp(decoded_key.value, "PING", strlen("PING")) == 0) {
        struct zcbor_string ping_val;
        success = zcbor_tstr_decode(decoding_state, &ping_val);
        if (!success) {
            goto error;
        }
        
        if (!strncmp(ping_val.value, "REQUEST", ping_val.len)) {
            /* 发送ping响应通知 */
            ping_notify();
        } else {
            LOG_ERR("Invalid ping value");
            goto error;
        }
    }

    /* 结束解码映射 */
    success = zcbor_map_end_decode(decoding_state);
    if (!success) {
        goto error;
    }

    return len;

error:
    LOG_ERR("PING CBOR decoding failed: %d", zcbor_peek_error(decoding_state));
    return -1;
}

/* CCC配置变更回调 */
static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);
    notify_enable = (value == BT_GATT_CCC_NOTIFY);
    LOG_DBG("Notification %s", notify_enable ? "enabled" : "disabled");
}

/* 设备信息响应 */
static int device_info_resp(void)
{
    uint8_t cbor_payload[256];
    bool success;

    /* 初始化CBOR编码状态 */
    ZCBOR_STATE_E(encoding_state, 1, cbor_payload, sizeof(cbor_payload), 0);

    /* 开始映射编码 */
    success = zcbor_map_start_encode(encoding_state, 6);
    if (!success) {
        goto error;
    }

    /* 添加版本信息 */
    success = zcbor_tstr_put_lit(encoding_state, "VERSION");
    if (!success) {
        goto error;
    }

    char version_str[16];
    sprintf(version_str, "%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
    /* 修复：添加长度参数 */
    success = zcbor_tstr_put_term(encoding_state, version_str, strlen(version_str));
    if (!success) {
        goto error;
    }

    /* 添加状态信息 */
    success = zcbor_tstr_put_lit(encoding_state, "STATUS");
    if (!success) {
        goto error;
    }

    success = zcbor_tstr_put_lit(encoding_state, "OK");
    if (!success) {
        goto error;
    }

    /* 添加序列号 */
    success = zcbor_tstr_put_lit(encoding_state, "SERIAL_NUM");
    if (!success) {
        goto error;
    }

    const char *serial = uicr_serial_number_get();
    /* 修复：添加长度参数 */
    success = zcbor_tstr_put_term(encoding_state, serial, strlen(serial));
    if (!success) {
        goto error;
    }

    /* 结束映射编码 */
    success = zcbor_map_end_encode(encoding_state, 6);
    if (!success) {
        goto error;
    }

    size_t cbor_len = encoding_state->payload - cbor_payload;
    memcpy(read_response.response, cbor_payload, cbor_len);
    read_response.response_len = cbor_len;
    
    return 0;

error:
    LOG_ERR("CBOR encoding failed: %d", zcbor_peek_error(encoding_state));
    return -1;
}

/* 从中央设备读取数据 */
static ssize_t read_from_central(
    struct bt_conn *conn, const struct bt_gatt_attr *attr, 
    void *buf, uint16_t len, uint16_t offset)
{
    char str[BT_UUID_STR_LEN];
    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    if (!bt_uuid_cmp(attr->uuid, &device_info_rx_uuid.uuid)) {
        device_info_resp();
    }

    LOG_DBG("Reading %d bytes from UUID: %s", read_response.response_len, str);
    
    return bt_gatt_attr_read(conn, attr, buf, len, offset, 
                           read_response.response, read_response.response_len);
}

/* GATT服务定义 */
BT_GATT_SERVICE_DEFINE(
    tracker_service,
    BT_GATT_PRIMARY_SERVICE(&tracker_service_uuid),

    /* 通知特征值 */
    BT_GATT_CHARACTERISTIC(&service_notify_uuid.uuid, BT_GATT_CHRC_NOTIFY, 
                          BT_GATT_PERM_READ, NULL, NULL, NULL),

    /* 通知CCC */
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* 设备信息读特征值 */
    BT_GATT_CHARACTERISTIC(&device_info_rx_uuid.uuid, BT_GATT_CHRC_READ, 
                          BT_GATT_PERM_READ, read_from_central, NULL, NULL),

    /* 设备信息写特征值 */
    BT_GATT_CHARACTERISTIC(&set_device_info_tx_uuid.uuid, BT_GATT_CHRC_WRITE, 
                          BT_GATT_PERM_WRITE, NULL, set_device_info, NULL),

    /* Ping特征值（需要加密） */
    BT_GATT_CHARACTERISTIC(&set_ping_tx_uuid.uuid, BT_GATT_CHRC_WRITE, 
                          BT_GATT_PERM_WRITE_ENCRYPT, NULL, set_ping, NULL),
);

/* Ping响应通知 */
static int ping_notify(void)
{
    if (!notify_enable) {
        LOG_WRN("Notifications not enabled");
        return -EACCES;
    }

    uint8_t cbor_payload[64];
    bool success;

    /* 初始化CBOR编码状态 */
    ZCBOR_STATE_E(encoding_state, 1, cbor_payload, sizeof(cbor_payload), 0);

    /* 一个键值对 */
    success = zcbor_map_start_encode(encoding_state, 2);
    if (!success) {
        goto error;
    }

    /* 添加ping响应键 */
    success = zcbor_tstr_put_lit(encoding_state, "PING");
    if (!success) {
        goto error;
    }

    /* 添加ping响应值 */
    success = zcbor_tstr_put_lit(encoding_state, "RESPONSE");
    if (!success) {
        goto error;
    }

    /* 结束映射 */
    success = zcbor_map_end_encode(encoding_state, 2);
    if (!success) {
        goto error;
    }

    size_t cbor_len = encoding_state->payload - cbor_payload;

    struct bt_conn *conn = ble_get_conn();
    if (conn) {
        int err = bt_gatt_notify(conn, &tracker_service.attrs[1], cbor_payload, cbor_len);
        if (err) {
            LOG_ERR("Notify failed: %d", err);
            return err;
        }
        LOG_DBG("Ping response sent");
    }

    return 0;

error:
    LOG_ERR("CBOR encoding failed: %d", zcbor_peek_error(encoding_state));
    return -1;
}

/* FOTA状态通知 */
int fota_notify(char *fota_state)
{
    if (!notify_enable) {
        return -EACCES;
    }

    uint8_t cbor_payload[128];
    bool success;

    /* 初始化CBOR编码状态 */
    ZCBOR_STATE_E(encoding_state, 1, cbor_payload, sizeof(cbor_payload), 0);

    /* 一个键值对 */
    success = zcbor_map_start_encode(encoding_state, 2);
    if (!success) {
        goto error;
    }

    success = zcbor_tstr_put_lit(encoding_state, "FOTA_STATUS");
    if (!success) {
        goto error;
    }

    /* 修复：添加长度参数 */
    success = zcbor_tstr_put_term(encoding_state, fota_state, strlen(fota_state));
    if (!success) {
        goto error;
    }

    success = zcbor_map_end_encode(encoding_state, 2);
    if (!success) {
        goto error;
    }

    size_t cbor_len = encoding_state->payload - cbor_payload;

    struct bt_conn *conn = ble_get_conn();
    if (conn) {
        return bt_gatt_notify(conn, &tracker_service.attrs[1], cbor_payload, cbor_len);
    }

    return 0;

error:
    LOG_ERR("CBOR encoding failed: %d", zcbor_peek_error(encoding_state));
    return -1;
}

/* 检查是否已配对 */
bool tracker_service_is_onboarded(void)
{
    bool onboarded = false;
    size_t len = sizeof(onboarded);
    
    /* 修复：使用正确的Settings API */
    int ret = settings_runtime_get(ONBOARDED_SETTINGS_PATH, &onboarded, len);
    
    if (ret < 0) {
        LOG_DBG("Failed to load onboarded setting, assuming not onboarded");
        return false;
    }
    
    return onboarded;
}

/* Tracker服务初始化 */
int tracker_service_init(void)
{
    LOG_INF("Initializing Tracker Service...");
    LOG_INF("Onboarded status: %s", tracker_service_is_onboarded() ? "Yes" : "No");
    return 0;
}