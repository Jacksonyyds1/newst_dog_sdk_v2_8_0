/* src/main.c */
/*
 * Copyright (c) 2024 Dog Tracker Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>

#include "ble.h"
#include "tracker_service.h"
#include "app_version.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

/* LED设备节点 */
#define LED_BLE_NODE DT_ALIAS(ble_led)
#define LED_STATUS_NODE DT_ALIAS(status_led)

/* 按钮设备节点 */
#define BUTTON_PAIRING_NODE DT_ALIAS(pairing_button)

static const struct gpio_dt_spec led_ble = GPIO_DT_SPEC_GET(LED_BLE_NODE, gpios);
static const struct gpio_dt_spec led_status = GPIO_DT_SPEC_GET(LED_STATUS_NODE, gpios);
static const struct gpio_dt_spec button_pairing = GPIO_DT_SPEC_GET(BUTTON_PAIRING_NODE, gpios);

/* LED状态枚举 */
typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK_SLOW,
    LED_STATE_BLINK_FAST
} led_state_t;

static led_state_t ble_led_state = LED_STATE_OFF;
static led_state_t status_led_state = LED_STATE_OFF;

/* LED控制定时器 */
static void led_timer_handler(struct k_timer *dummy);
K_TIMER_DEFINE(led_timer, led_timer_handler, NULL);

/* 按钮中断处理 */
static struct gpio_callback button_cb_data;
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/* LED控制函数 */
static void set_led_state(const struct gpio_dt_spec *led, led_state_t state)
{
    if (led == &led_ble) {
        ble_led_state = state;
    } else if (led == &led_status) {
        status_led_state = state;
    }
    
    switch (state) {
        case LED_STATE_OFF:
            gpio_pin_set_dt(led, 0);
            break;
        case LED_STATE_ON:
            gpio_pin_set_dt(led, 1);
            break;
        case LED_STATE_BLINK_SLOW:
        case LED_STATE_BLINK_FAST:
            /* 定时器处理闪烁 */
            break;
    }
}

/* LED定时器处理函数 */
static void led_timer_handler(struct k_timer *dummy)
{
    static bool toggle = false;
    
    toggle = !toggle;
    
    /* BLE LED闪烁处理 */
    if (ble_led_state == LED_STATE_BLINK_SLOW || ble_led_state == LED_STATE_BLINK_FAST) {
        gpio_pin_set_dt(&led_ble, toggle);
    }
    
    /* 状态LED闪烁处理 */
    if (status_led_state == LED_STATE_BLINK_SLOW || status_led_state == LED_STATE_BLINK_FAST) {
        gpio_pin_set_dt(&led_status, toggle);
    }
}

/* 按钮中断处理函数 */
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("Pairing button pressed");
    
    /* 开始BLE广播，持续10分钟 */
    int ret = ble_advertise_start(600);
    if (ret == 0) {
        set_led_state(&led_ble, LED_STATE_BLINK_SLOW);
        LOG_INF("BLE advertising started for 10 minutes");
    } else {
        LOG_ERR("Failed to start BLE advertising: %d", ret);
        set_led_state(&led_status, LED_STATE_BLINK_FAST);
    }
}

/* 系统初始化 */
static int system_init(void)
{
    int ret;
    
    LOG_INF("Dog Tracker v%d.%d.%d initializing...", 
            APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
    
    /* 初始化LED */
    if (!gpio_is_ready_dt(&led_ble)) {
        LOG_ERR("BLE LED device not ready");
        return -ENODEV;
    }
    
    if (!gpio_is_ready_dt(&led_status)) {
        LOG_ERR("Status LED device not ready");
        return -ENODEV;
    }
    
    ret = gpio_pin_configure_dt(&led_ble, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure BLE LED: %d", ret);
        return ret;
    }
    
    ret = gpio_pin_configure_dt(&led_status, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure status LED: %d", ret);
        return ret;
    }
    
    /* 初始化按钮 */
    if (!gpio_is_ready_dt(&button_pairing)) {
        LOG_ERR("Pairing button device not ready");
        return -ENODEV;
    }
    
    ret = gpio_pin_configure_dt(&button_pairing, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Failed to configure pairing button: %d", ret);
        return ret;
    }
    
    ret = gpio_pin_interrupt_configure_dt(&button_pairing, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure button interrupt: %d", ret);
        return ret;
    }
    
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button_pairing.pin));
    gpio_add_callback(button_pairing.port, &button_cb_data);
    
    /* 启动LED定时器 */
    k_timer_start(&led_timer, K_MSEC(500), K_MSEC(500));
    
    /* 初始化BLE */
    ret = ble_init();
    if (ret != 0) {
        LOG_ERR("BLE initialization failed: %d", ret);
        set_led_state(&led_status, LED_STATE_BLINK_FAST);
        return ret;
    }
    
    /* 初始化Tracker服务 */
    ret = tracker_service_init();
    if (ret != 0) {
        LOG_ERR("Tracker service initialization failed: %d", ret);
        return ret;
    }
    
    /* 启动时指示 */
    set_led_state(&led_status, LED_STATE_ON);
    k_sleep(K_SECONDS(2));
    set_led_state(&led_status, LED_STATE_OFF);
    
    LOG_INF("System initialization completed successfully");
    return 0;
}

/* 主线程 */
int main(void)
{
    int ret;
    
    ret = system_init();
    if (ret != 0) {
        LOG_ERR("System initialization failed: %d", ret);
        /* 错误指示 */
        while (1) {
            gpio_pin_toggle_dt(&led_status);
            k_sleep(K_MSEC(200));
        }
    }
    
    LOG_INF("Dog Tracker running...");
    
    /* 主循环 */
    while (1) {
        /* 检查连接状态 */
        struct bt_conn *conn = ble_get_conn();
        if (conn != NULL) {
            /* 已连接，BLE LED常亮 */
            if (ble_led_state != LED_STATE_ON) {
                set_led_state(&led_ble, LED_STATE_ON);
            }
        } else {
            /* 未连接 */
            if (ble_led_state == LED_STATE_ON) {
                set_led_state(&led_ble, LED_STATE_OFF);
            }
        }
        
        /* 系统状态检查 */
        k_sleep(K_SECONDS(1));
    }
    
    return 0;
}