//==============================================================================
// nRF54L15 蓝牙与App配对连接 + Shell控制 
// 基于 nRF Connect SDK v2.8.0
//==============================================================================

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/settings/settings.h>

#include <bluetooth/services/nus.h>
#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

//==============================================================================
// 配置和常量定义
//==============================================================================

#define DEVICE_NAME             CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

// LED和按键定义
#define RUN_STATUS_LED          DK_LED1
#define CON_STATUS_LED          DK_LED2
#define USER_LED                DK_LED3
#define RUN_LED_BLINK_INTERVAL  1000

// 数据缓冲区大小
#define MAX_DATA_LEN           240

//==============================================================================
// 全局变量
//==============================================================================

static struct bt_conn *current_conn;
static struct bt_conn *auth_conn;
static uint32_t packet_count = 0;
static bool bt_ready = false;
static bool auto_blink = true;
static bool led_states[4] = {false, false, false, false};

//==============================================================================
// 蓝牙广播数据配置
//==============================================================================

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

//==============================================================================
// Shell命令实现
//==============================================================================

static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: led on <1-4>");
        return -EINVAL;
    }
    
    int led_num = atoi(argv[1]);
    if (led_num < 1 || led_num > 4) {
        shell_error(sh, "LED number must be 1-4");
        return -EINVAL;
    }
    
    dk_set_led_on(led_num - 1);
    led_states[led_num - 1] = true;
    shell_print(sh, "LED %d turned ON", led_num);
    
    if (current_conn) {
        char msg[32];
        snprintf(msg, sizeof(msg), "LED_%d_ON", led_num);
        bt_nus_send(NULL, msg, strlen(msg));
    }
    
    return 0;
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: led off <1-4>");
        return -EINVAL;
    }
    
    int led_num = atoi(argv[1]);
    if (led_num < 1 || led_num > 4) {
        shell_error(sh, "LED number must be 1-4");
        return -EINVAL;
    }
    
    dk_set_led_off(led_num - 1);
    led_states[led_num - 1] = false;
    shell_print(sh, "LED %d turned OFF", led_num);
    
    if (current_conn) {
        char msg[32];
        snprintf(msg, sizeof(msg), "LED_%d_OFF", led_num);
        bt_nus_send(NULL, msg, strlen(msg));
    }
    
    return 0;
}

static int cmd_led_toggle(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: led toggle <1-4>");
        return -EINVAL;
    }
    
    int led_num = atoi(argv[1]);
    if (led_num < 1 || led_num > 4) {
        shell_error(sh, "LED number must be 1-4");
        return -EINVAL;
    }
    
    led_states[led_num - 1] = !led_states[led_num - 1];
    dk_set_led(led_num - 1, led_states[led_num - 1]);
    shell_print(sh, "LED %d toggled to %s", led_num, led_states[led_num - 1] ? "ON" : "OFF");
    
    return 0;
}

static int cmd_bt_status(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "=== Bluetooth Status ===");
    shell_print(sh, "Device Name: %s", DEVICE_NAME);
    shell_print(sh, "Ready: %s", bt_ready ? "Yes" : "No");
    shell_print(sh, "Connected: %s", current_conn ? "Yes" : "No");
    shell_print(sh, "Packets sent: %d", packet_count);
    
    if (current_conn) {
        char addr[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(bt_conn_get_dst(current_conn), addr, sizeof(addr));
        shell_print(sh, "Connected to: %s", addr);
    }
    
    return 0;
}

static int cmd_bt_send(const struct shell *sh, size_t argc, char **argv)
{
    if (!current_conn) {
        shell_error(sh, "No bluetooth connection");
        return -ENOTCONN;
    }
    
    if (argc < 2) {
        shell_error(sh, "Usage: bt send <message>");
        return -EINVAL;
    }
    
    char message[MAX_DATA_LEN] = {0};
    int pos = 0;
    
    for (int i = 1; i < argc && pos < (MAX_DATA_LEN - 1); i++) {
        if (i > 1) {
            message[pos++] = ' ';
        }
        int len = strlen(argv[i]);
        if (pos + len < MAX_DATA_LEN - 1) {
            strcpy(&message[pos], argv[i]);
            pos += len;
        }
    }
    
    int ret = bt_nus_send(NULL, message, strlen(message));
    if (ret) {
        shell_error(sh, "Failed to send message (err %d)", ret);
        return ret;
    }
    
    shell_print(sh, "Sent: %s", message);
    return 0;
}

static int cmd_bt_disconnect(const struct shell *sh, size_t argc, char **argv)
{
    if (!current_conn) {
        shell_error(sh, "No active connection");
        return -ENOTCONN;
    }
    
    bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    shell_print(sh, "Disconnection initiated");
    return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "=== nRF54L15 System Info ===");
    shell_print(sh, "Device: %s", DEVICE_NAME);
    shell_print(sh, "SDK: nRF Connect SDK v2.8.0");
    shell_print(sh, "Chip: nRF54L15");
    shell_print(sh, "Uptime: %u ms", k_uptime_get_32());
    shell_print(sh, "System heap: %d bytes", CONFIG_HEAP_MEM_POOL_SIZE);
    shell_print(sh, "LED States: LED1=%s, LED2=%s, LED3=%s, LED4=%s", 
                led_states[0] ? "ON" : "OFF",
                led_states[1] ? "ON" : "OFF", 
                led_states[2] ? "ON" : "OFF",
                led_states[3] ? "ON" : "OFF");
    return 0;
}

static int cmd_blink(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_error(sh, "Usage: blink <on|off>");
        return -EINVAL;
    }
    
    if (strcmp(argv[1], "on") == 0) {
        auto_blink = true;
        shell_print(sh, "Auto LED blink enabled");
    } else if (strcmp(argv[1], "off") == 0) {
        auto_blink = false;
        shell_print(sh, "Auto LED blink disabled");
    } else {
        shell_error(sh, "Invalid argument. Use 'on' or 'off'");
        return -EINVAL;
    }
    
    return 0;
}

//==============================================================================
// Shell命令注册
//==============================================================================

SHELL_STATIC_SUBCMD_SET_CREATE(led_cmds,
    SHELL_CMD_ARG(on, NULL, "Turn LED on <1-4>", cmd_led_on, 2, 0),
    SHELL_CMD_ARG(off, NULL, "Turn LED off <1-4>", cmd_led_off, 2, 0),
    SHELL_CMD_ARG(toggle, NULL, "Toggle LED <1-4>", cmd_led_toggle, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(bt_cmds,
    SHELL_CMD(status, NULL, "Show bluetooth status", cmd_bt_status),
    SHELL_CMD_ARG(send, NULL, "Send message via BT", cmd_bt_send, 2, 10),
    SHELL_CMD(disconnect, NULL, "Disconnect bluetooth", cmd_bt_disconnect),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led, &led_cmds, "LED control commands", NULL);
SHELL_CMD_REGISTER(bt, &bt_cmds, "Bluetooth commands", NULL);
SHELL_CMD_REGISTER(info, NULL, "System information", cmd_info);
SHELL_CMD_ARG_REGISTER(blink, NULL, "Control auto LED blink", cmd_blink, 2, 0);

//==============================================================================
// NUS服务回调函数
//==============================================================================

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    char addr[BT_ADDR_LE_STR_LEN] = {0};
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, ARRAY_SIZE(addr));
    
    LOG_INF("Received data from %s: %.*s", addr, len, data);
    printk("BT RX [%s]: %.*s\n", addr, len, data);
    
    // LED控制命令处理
    if (strncmp((char*)data, "LED_ON_", 7) == 0) {
        int led = data[7] - '0';
        if (led >= 1 && led <= 4) {
            dk_set_led_on(led - 1);
            led_states[led - 1] = true;
            bt_nus_send(NULL, "LED_ON_OK", 9);
            printk("LED %d turned ON via BT\n", led);
        }
    } else if (strncmp((char*)data, "LED_OFF_", 8) == 0) {
        int led = data[8] - '0';
        if (led >= 1 && led <= 4) {
            dk_set_led_off(led - 1);
            led_states[led - 1] = false;
            bt_nus_send(NULL, "LED_OFF_OK", 10);
            printk("LED %d turned OFF via BT\n", led);
        }
    } else if (strncmp((char*)data, "STATUS", 6) == 0) {
        char status[128];
        int status_len = snprintf(status, sizeof(status), 
                                 "Device:%s,Packets:%d,Uptime:%u,LEDs:%d%d%d%d", 
                                 DEVICE_NAME, packet_count, k_uptime_get_32(),
                                 led_states[0] ? 1 : 0, led_states[1] ? 1 : 0,
                                 led_states[2] ? 1 : 0, led_states[3] ? 1 : 0);
        bt_nus_send(NULL, status, status_len);
    } else {
        // 默认回显
        char response[MAX_DATA_LEN];
        int response_len = snprintf(response, sizeof(response), 
                                   "Echo #%d: %.*s", ++packet_count, len, data);
        
        if (response_len > 0 && response_len < sizeof(response)) {
            bt_nus_send(NULL, response, response_len);
        }
    }
}

static void bt_sent_cb(struct bt_conn *conn)
{
    LOG_DBG("Data sent to bluetooth device");
}

static void bt_send_enabled_cb(enum bt_nus_send_status status)
{
    if (status == BT_NUS_SEND_STATUS_ENABLED) {
        LOG_INF("Bluetooth TX notifications enabled");
        printk("BT: TX notifications enabled\n");
    } else {
        LOG_INF("Bluetooth TX notifications disabled");
        printk("BT: TX notifications disabled\n");
    }
}

static struct bt_nus_cb nus_cb = {
    .received = bt_receive_cb,
    .sent = bt_sent_cb,
    .send_enabled = bt_send_enabled_cb,
};

//==============================================================================
// 蓝牙连接管理
//==============================================================================

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        LOG_ERR("Connection failed, err 0x%02x %s", err, bt_hci_err_to_str(err));
        printk("BT: Connection failed (err %d)\n", err);
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected to %s", addr);
    printk("BT: Connected to %s\n", addr);

    current_conn = bt_conn_ref(conn);
    dk_set_led_on(CON_STATUS_LED);
    led_states[CON_STATUS_LED] = true;
    
    // 发送连接成功消息
    k_sleep(K_MSEC(100));
    char welcome_msg[64];
    int msg_len = snprintf(welcome_msg, sizeof(welcome_msg), 
                          "Welcome to %s! Shell ready.", DEVICE_NAME);
    if (msg_len > 0) {
        bt_nus_send(NULL, welcome_msg, msg_len);
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Disconnected: %s, reason 0x%02x %s", addr, reason, bt_hci_err_to_str(reason));
    printk("BT: Disconnected from %s (reason %d)\n", addr, reason);

    if (auth_conn) {
        bt_conn_unref(auth_conn);
        auth_conn = NULL;
    }

    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
        dk_set_led_off(CON_STATUS_LED);
        led_states[CON_STATUS_LED] = false;
    }
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
                           enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_INF("Security changed: %s level %u", addr, level);
        printk("BT: Security changed for %s (level %u)\n", addr, level);
    } else {
        LOG_WRN("Security failed: %s level %u err %d %s", addr, level, err,
               bt_security_err_to_str(err));
        printk("BT: Security failed for %s\n", addr);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
    .security_changed = security_changed,
};

//==============================================================================
// 蓝牙认证和配对
//==============================================================================

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Passkey for %s: %06u", addr, passkey);
    printk("BT: Passkey for %s: %06u\n", addr, passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    auth_conn = bt_conn_ref(conn);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Passkey for %s: %06u", addr, passkey);
    printk("BT: Passkey for %s: %06u\n", addr, passkey);
    printk("Press Button 1 to confirm, Button 2 to reject.\n");
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Pairing cancelled: %s", addr);
    printk("BT: Pairing cancelled for %s\n", addr);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
    printk("BT: Pairing completed for %s (bonded: %d)\n", addr, bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_WRN("Pairing failed conn: %s, reason %d %s", addr, reason,
           bt_security_err_to_str(reason));
    printk("BT: Pairing failed for %s (reason %d)\n", addr, reason);
}

static struct bt_conn_auth_cb conn_auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed
};

//==============================================================================
// 按键处理
//==============================================================================

static void button_changed(uint32_t button_state, uint32_t has_changed)
{
    uint32_t buttons = button_state & has_changed;

    if (auth_conn) {
        if (buttons & DK_BTN1_MSK) {
            bt_conn_auth_pairing_confirm(auth_conn);
            LOG_INF("Pairing confirmed");
            printk("BT: Pairing confirmed\n");
            bt_conn_unref(auth_conn);
            auth_conn = NULL;
        } else if (buttons & DK_BTN2_MSK) {
            bt_conn_auth_cancel(auth_conn);
            LOG_INF("Pairing cancelled");
            printk("BT: Pairing cancelled\n");
            bt_conn_unref(auth_conn);
            auth_conn = NULL;
        }
    } else {
        if (buttons & DK_BTN1_MSK) {
            LOG_INF("Button 1 pressed");
            printk("Button 1 pressed\n");
            if (current_conn) {
                bt_nus_send(NULL, "BTN1_PRESSED", 12);
            }
        }
        
        if (buttons & DK_BTN2_MSK) {
            LOG_INF("Button 2 pressed");
            printk("Button 2 pressed\n");
            if (current_conn) {
                bt_nus_send(NULL, "BTN2_PRESSED", 12);
            }
        }
    }
}

//==============================================================================
// 主函数
//==============================================================================

int main(void)
{
    int blink_status = 0;
    int err = 0;

    printk("\n");
    printk("=== nRF54L15 Bluetooth + Shell Demo ===\n");
    printk("Device: %s\n", DEVICE_NAME);
    printk("SDK Version: nRF Connect SDK v2.8.0\n");
    printk("Features: Bluetooth LE, Shell, LED Control\n");
    printk("\n");

    LOG_INF("Starting nRF54L15 Bluetooth + Shell Demo");

    // 初始化LED和按键
    err = dk_leds_init();
    if (err) {
        printk("ERROR: LEDs init failed (err %d)\n", err);
        return 0;
    }
    printk("LEDs initialized successfully\n");

    err = dk_buttons_init(button_changed);
    if (err) {
        printk("ERROR: Cannot init buttons (err: %d)\n", err);
    } else {
        printk("Buttons initialized successfully\n");
    }

    // 初始化蓝牙堆栈
    printk("Initializing Bluetooth...\n");
    err = bt_enable(NULL);
    if (err) {
        printk("ERROR: Bluetooth init failed (err %d)\n", err);
        return 0;
    }
    printk("Bluetooth initialized successfully\n");
    bt_ready = true;

    // 载入保存的设置
    if (IS_ENABLED(CONFIG_SETTINGS)) {
        printk("Loading settings...\n");
        settings_load();
    }

    // 注册连接认证回调
    err = bt_conn_auth_cb_register(&conn_auth_callbacks);
    if (err) {
        printk("ERROR: Failed to register auth callbacks\n");
        return 0;
    }

    err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
    if (err) {
        printk("ERROR: Failed to register auth info callbacks\n");
        return 0;
    }

    // 初始化NUS服务
    printk("Initializing NUS service...\n");
    err = bt_nus_init(&nus_cb);
    if (err) {
        printk("ERROR: Failed to initialize NUS service (err: %d)\n", err);
        return 0;
    }

    // 开始广播
    printk("Starting advertising...\n");
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) {
        printk("ERROR: Advertising failed to start (err %d)\n", err);
        return 0;
    }

    printk("=== System Ready ===\n");
    printk("Advertising started - Device discoverable\n");
    printk("Shell commands available:\n");
    printk("  led on/off/toggle <1-4>  - Control LEDs\n");
    printk("  bt status/send/disconnect - Bluetooth control\n");
    printk("  info                     - System information\n");
    printk("  blink on/off             - Auto LED blink control\n");
    printk("LED Status: LED1=Run, LED2=Connected, LED3=User\n");
    printk("Ready for connections and shell commands!\n");
    printk("\n");

    LOG_INF("System ready - Advertising started");

    // 主循环 - LED状态指示
    for (;;) {
        if (auto_blink) {
            dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
            led_states[RUN_STATUS_LED] = (blink_status % 2);
        }
        
        // 每30秒打印一次状态 (仅在没有活动连接时)
        if ((blink_status % 30) == 0 && !current_conn) {
            printk("System running... Waiting for BT connection (uptime: %u ms)\n", 
                   k_uptime_get_32());
        }
        
        k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
    }

    return 0;
}