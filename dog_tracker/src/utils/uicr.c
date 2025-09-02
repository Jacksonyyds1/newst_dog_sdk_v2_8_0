/* src/utils/uicr.c */
/*
 * Copyright (c) 2024 Dog Tracker Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include <stdio.h>

#include "uicr.h"

LOG_MODULE_REGISTER(uicr, CONFIG_LOG_DEFAULT_LEVEL);

/* 模拟的序列号存储 */
static char device_serial[UICR_STR_MAX_LEN] = "DT24001234AB";

/* 模拟的WiFi MAC地址 */
static char wifi_mac[18] = "AA:BB:CC:DD:EE:FF";

/* 出厂标志位 - 可通过Shell命令或设置修改 */
static bool shipping_mode = true;

char *uicr_serial_number_get(void)
{
    return device_serial;
}

bool uicr_shipping_flag_get(void)
{
    return shipping_mode;
}

char *uicr_wifi_mac_address_get(void)
{
    return wifi_mac;
}

/* Shell命令支持 - 用于开发调试 */
#include <zephyr/shell/shell.h>

static int uicr_serial_cmd(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1) {
        shell_print(sh, "Current serial: %s", device_serial);
        return 0;
    }
    
    if (argc == 2) {
        if (strlen(argv[1]) >= UICR_STR_MAX_LEN) {
            shell_error(sh, "Serial number too long (max %d chars)", 
                       UICR_STR_MAX_LEN - 1);
            return -EINVAL;
        }
        
        strncpy(device_serial, argv[1], UICR_STR_MAX_LEN - 1);
        device_serial[UICR_STR_MAX_LEN - 1] = '\0';
        shell_print(sh, "Serial number set to: %s", device_serial);
        return 0;
    }
    
    shell_error(sh, "Usage: uicr serial [new_serial]");
    return -EINVAL;
}

static int uicr_shipping_cmd(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1) {
        shell_print(sh, "Shipping mode: %s", shipping_mode ? "enabled" : "disabled");
        return 0;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "1") == 0) {
            shipping_mode = true;
            shell_print(sh, "Shipping mode enabled");
        } else if (strcmp(argv[1], "off") == 0 || strcmp(argv[1], "0") == 0) {
            shipping_mode = false;
            shell_print(sh, "Shipping mode disabled");
        } else {
            shell_error(sh, "Use 'on' or 'off'");
            return -EINVAL;
        }
        return 0;
    }
    
    shell_error(sh, "Usage: uicr shipping [on|off]");
    return -EINVAL;
}

static int uicr_mac_cmd(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1) {
        shell_print(sh, "WiFi MAC: %s", wifi_mac);
        return 0;
    }
    
    if (argc == 2) {
        if (strlen(argv[1]) >= sizeof(wifi_mac)) {
            shell_error(sh, "MAC address too long");
            return -EINVAL;
        }
        
        strncpy(wifi_mac, argv[1], sizeof(wifi_mac) - 1);
        wifi_mac[sizeof(wifi_mac) - 1] = '\0';
        shell_print(sh, "WiFi MAC set to: %s", wifi_mac);
        return 0;
    }
    
    shell_error(sh, "Usage: uicr mac [AA:BB:CC:DD:EE:FF]");
    return -EINVAL;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_uicr,
    SHELL_CMD(serial, NULL, "Get/set device serial number", uicr_serial_cmd),
    SHELL_CMD(shipping, NULL, "Get/set shipping mode", uicr_shipping_cmd),
    SHELL_CMD(mac, NULL, "Get/set WiFi MAC address", uicr_mac_cmd),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(uicr, &sub_uicr, "UICR utilities", NULL);