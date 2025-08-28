/*
 * 按键控制LED示例程序 - nRF54L15DK
 * 功能：按下不同按键控制对应LED的开关状态
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(button_led, LOG_LEVEL_DBG);

/* LED定义 - 基于设备树别名 */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* 按键定义 - 基于设备树别名 */
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

/* GPIO设备结构体 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);

/* LED状态变量 */
static bool led_states[4] = {false, false, false, false};

/* GPIO回调结构体 */
static struct gpio_callback button0_cb_data;
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;

/* 按键回调函数 */
void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    led_states[0] = !led_states[0];
    gpio_pin_set_dt(&led0, led_states[0]);
    LOG_INF("Button 0 pressed, LED 0 %s", led_states[0] ? "ON" : "OFF");
}

void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    led_states[1] = !led_states[1];
    gpio_pin_set_dt(&led1, led_states[1]);
    LOG_INF("Button 1 pressed, LED 1 %s", led_states[1] ? "ON" : "OFF");
}

void button2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    led_states[2] = !led_states[2];
    gpio_pin_set_dt(&led2, led_states[2]);
    LOG_INF("Button 2 pressed, LED 2 %s", led_states[2] ? "ON" : "OFF");
}

void button3_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    led_states[3] = !led_states[3];
    gpio_pin_set_dt(&led3, led_states[3]);
    LOG_INF("Button 3 pressed, LED 3 %s", led_states[3] ? "ON" : "OFF");
}

/* LED初始化函数 */
int init_leds(void)
{
    int ret;

    /* 检查LED GPIO设备是否就绪 */
    if (!device_is_ready(led0.port) || !device_is_ready(led1.port) ||
        !device_is_ready(led2.port) || !device_is_ready(led3.port)) {
        LOG_ERR("LED GPIO devices not ready");
        return -1;
    }

    /* 配置LED为输出模式 */
    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure LED 0 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure LED 1 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure LED 2 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure LED 3 (%d)", ret);
        return ret;
    }

    LOG_INF("LEDs initialized successfully");
    return 0;
}

/* 按键初始化函数 */
int init_buttons(void)
{
    int ret;

    /* 检查按键GPIO设备是否就绪 */
    if (!device_is_ready(button0.port) || !device_is_ready(button1.port) ||
        !device_is_ready(button2.port) || !device_is_ready(button3.port)) {
        LOG_ERR("Button GPIO devices not ready");
        return -1;
    }

    /* 配置按键为输入模式，启用内部上拉 */
    ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 0 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 1 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 2 (%d)", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 3 (%d)", ret);
        return ret;
    }

    /* 配置按键中断 */
    ret = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 0 interrupt (%d)", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 1 interrupt (%d)", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&button2, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 2 interrupt (%d)", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&button3, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot configure button 3 interrupt (%d)", ret);
        return ret;
    }

    /* 初始化并添加GPIO回调 */
    gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin));
    gpio_add_callback(button0.port, &button0_cb_data);

    gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
    gpio_add_callback(button1.port, &button1_cb_data);

    gpio_init_callback(&button2_cb_data, button2_pressed, BIT(button2.pin));
    gpio_add_callback(button2.port, &button2_cb_data);

    gpio_init_callback(&button3_cb_data, button3_pressed, BIT(button3.pin));
    gpio_add_callback(button3.port, &button3_cb_data);

    LOG_INF("Buttons initialized successfully");
    return 0;
}

/* LED闪烁演示函数 */
void led_demo(void)
{
    LOG_INF("Starting LED demo...");
    
    /* 依次点亮所有LED */
    for (int i = 0; i < 4; i++) {
        switch(i) {
            case 0: gpio_pin_set_dt(&led0, 1); break;
            case 1: gpio_pin_set_dt(&led1, 1); break;
            case 2: gpio_pin_set_dt(&led2, 1); break;
            case 3: gpio_pin_set_dt(&led3, 1); break;
        }
        k_msleep(200);
    }
    
    /* 依次关闭所有LED */
    for (int i = 0; i < 4; i++) {
        switch(i) {
            case 0: gpio_pin_set_dt(&led0, 0); break;
            case 1: gpio_pin_set_dt(&led1, 0); break;
            case 2: gpio_pin_set_dt(&led2, 0); break;
            case 3: gpio_pin_set_dt(&led3, 0); break;
        }
        k_msleep(200);
    }
    
    LOG_INF("LED demo complete");
}

/* 主函数 */
int main(void)
{
    int ret;

    LOG_INF("nRF54L15 Button LED Control Demo Starting...");

    /* 初始化LEDs */
    ret = init_leds();
    if (ret < 0) {
        LOG_ERR("LED initialization failed");
        return ret;
    }

    /* 初始化按键 */
    ret = init_buttons();
    if (ret < 0) {
        LOG_ERR("Button initialization failed");
        return ret;
    }

    /* 启动演示 */
    led_demo();

    LOG_INF("System ready! Press buttons to control LEDs:");
    LOG_INF("- Button 0 -> LED 0");
    LOG_INF("- Button 1 -> LED 1");
    LOG_INF("- Button 2 -> LED 2");
    LOG_INF("- Button 3 -> LED 3");

    /* 主循环 */
    while (1) {
        k_msleep(1000);
        /* 可以在这里添加其他周期性任务 */
    }

    return 0;
}