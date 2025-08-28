/*
 * 
 * 功能：按下不同按键控制对应LED的开关状态 + Shell命令控制
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

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
static const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios)
};

static const struct gpio_dt_spec buttons[] = {
    GPIO_DT_SPEC_GET(SW0_NODE, gpios),
    GPIO_DT_SPEC_GET(SW1_NODE, gpios),
    GPIO_DT_SPEC_GET(SW2_NODE, gpios),
    GPIO_DT_SPEC_GET(SW3_NODE, gpios)
};

#define NUM_LEDS ARRAY_SIZE(leds)
#define NUM_BUTTONS ARRAY_SIZE(buttons)

/* LED状态变量 */
static bool led_states[NUM_LEDS] = {false, false, false, false};
static uint32_t button_press_counts[NUM_BUTTONS] = {0, 0, 0, 0};

/* GPIO回调结构体 */
static struct gpio_callback button_cb_data[NUM_BUTTONS];

/* 按键回调函数 */
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (BIT(buttons[i].pin) & pins) {
            led_states[i] = !led_states[i];
            /* 使用gpio_pin_set_dt确保正确设置LED状态 */
            gpio_pin_set_dt(&leds[i], led_states[i]);
            button_press_counts[i]++;
            LOG_INF("Button %d pressed (count: %d), LED %d %s", 
                    i, button_press_counts[i], i, led_states[i] ? "ON" : "OFF");
            break;
        }
    }
}

/* LED控制函数 */
static int set_led(int led_num, bool state)
{
    int ret;
    
    if (led_num < 0 || led_num >= NUM_LEDS) {
        return -EINVAL;
    }
    
    /* 更新内部状态 */
    led_states[led_num] = state;
    
    /* 设置GPIO状态 - 使用gpio_pin_set_dt */
    ret = gpio_pin_set_dt(&leds[led_num], state);
    
    /* 添加调试信息 */
    LOG_DBG("Setting LED %d to %d (state=%s), GPIO port=%p pin=%d, ret=%d",
            led_num, state, state ? "ON" : "OFF", 
            leds[led_num].port, leds[led_num].pin, ret);
    
    /* 验证GPIO状态 */
    int actual_state = gpio_pin_get_dt(&leds[led_num]);
    LOG_DBG("LED %d actual GPIO state: %d", led_num, actual_state);
    
    return ret;
}

static bool get_led_state(int led_num)
{
    if (led_num < 0 || led_num >= NUM_LEDS) {
        return false;
    }
    return led_states[led_num];
}

/* GPIO状态读取函数 - 用于调试 */
static int get_gpio_state(int led_num)
{
    if (led_num < 0 || led_num >= NUM_LEDS) {
        return -1;
    }
    return gpio_pin_get_dt(&leds[led_num]);
}

/* Shell命令实现 */

/* LED控制命令: led <led_num> <on|off|toggle> */
static int cmd_led(const struct shell *sh, size_t argc, char **argv)
{
    int led_num;
    int ret;

    if (argc < 3) {
        shell_error(sh, "Usage: led <led_num> <on|off|toggle>");
        shell_print(sh, "  led_num: 0-3");
        shell_print(sh, "  action: on, off, toggle");
        return -EINVAL;
    }

    led_num = atoi(argv[1]);
    if (led_num < 0 || led_num >= NUM_LEDS) {
        shell_error(sh, "LED number must be 0-%d", NUM_LEDS - 1);
        return -EINVAL;
    }

    if (strcmp(argv[2], "on") == 0) {
        ret = set_led(led_num, true);
        shell_print(sh, "LED %d turned ON (ret=%d)", led_num, ret);
        shell_print(sh, "GPIO state: %d", get_gpio_state(led_num));
    } else if (strcmp(argv[2], "off") == 0) {
        ret = set_led(led_num, false);
        shell_print(sh, "LED %d turned OFF (ret=%d)", led_num, ret);
        shell_print(sh, "GPIO state: %d", get_gpio_state(led_num));
    } else if (strcmp(argv[2], "toggle") == 0) {
        bool new_state = !get_led_state(led_num);
        ret = set_led(led_num, new_state);
        shell_print(sh, "LED %d toggled to %s (ret=%d)", led_num, new_state ? "ON" : "OFF", ret);
        shell_print(sh, "GPIO state: %d", get_gpio_state(led_num));
    } else {
        shell_error(sh, "Invalid action. Use: on, off, or toggle");
        return -EINVAL;
    }

    if (ret < 0) {
        shell_error(sh, "Failed to control LED %d (error: %d)", led_num, ret);
    }

    return ret;
}

/* LED状态查询命令: led_status */
static int cmd_led_status(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "LED Status:");
    for (int i = 0; i < NUM_LEDS; i++) {
        int gpio_state = get_gpio_state(i);
        shell_print(sh, "  LED %d: %s (GPIO: %d)", i, led_states[i] ? "ON" : "OFF", gpio_state);
    }
    return 0;
}

/* GPIO调试命令: gpio_info */
static int cmd_gpio_info(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "GPIO Information:");
    for (int i = 0; i < NUM_LEDS; i++) {
        shell_print(sh, "  LED %d: Port=%p Pin=%d Ready=%s", 
                    i, leds[i].port, leds[i].pin,
                    device_is_ready(leds[i].port) ? "Yes" : "No");
    }
    return 0;
}

/* 强制GPIO控制命令: gpio_set <led_num> <0|1> */
static int cmd_gpio_set(const struct shell *sh, size_t argc, char **argv)
{
    int led_num, value, ret;
    
    if (argc < 3) {
        shell_error(sh, "Usage: gpio_set <led_num> <0|1>");
        return -EINVAL;
    }

    led_num = atoi(argv[1]);
    value = atoi(argv[2]);
    
    if (led_num < 0 || led_num >= NUM_LEDS) {
        shell_error(sh, "LED number must be 0-%d", NUM_LEDS - 1);
        return -EINVAL;
    }
    
    if (value != 0 && value != 1) {
        shell_error(sh, "Value must be 0 or 1");
        return -EINVAL;
    }

    /* 直接使用gpio_pin_set而不更新内部状态 */
    ret = gpio_pin_set(leds[led_num].port, leds[led_num].pin, value);
    
    shell_print(sh, "Direct GPIO set LED %d to %d, ret=%d", led_num, value, ret);
    shell_print(sh, "GPIO read back: %d", gpio_pin_get(leds[led_num].port, leds[led_num].pin));
    
    return ret;
}

/* 按键统计命令: button_stats */
static int cmd_button_stats(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "Button Press Statistics:");
    for (int i = 0; i < NUM_BUTTONS; i++) {
        shell_print(sh, "  Button %d: %d presses", i, button_press_counts[i]);
    }
    return 0;
}

/* LED闪烁命令: led_blink <led_num> <times> [interval_ms] */
static int cmd_led_blink(const struct shell *sh, size_t argc, char **argv)
{
    int led_num, times, interval = 500;
    
    if (argc < 3) {
        shell_error(sh, "Usage: led_blink <led_num> <times> [interval_ms]");
        return -EINVAL;
    }

    led_num = atoi(argv[1]);
    times = atoi(argv[2]);
    if (argc >= 4) {
        interval = atoi(argv[3]);
    }

    if (led_num < 0 || led_num >= NUM_LEDS) {
        shell_error(sh, "LED number must be 0-%d", NUM_LEDS - 1);
        return -EINVAL;
    }

    if (times <= 0 || times > 50) {
        shell_error(sh, "Blink times must be 1-50");
        return -EINVAL;
    }

    shell_print(sh, "Blinking LED %d for %d times (interval: %dms)", 
                led_num, times, interval);

    bool original_state = led_states[led_num];
    
    for (int i = 0; i < times; i++) {
        set_led(led_num, true);
        k_msleep(interval);
        set_led(led_num, false);
        k_msleep(interval);
    }
    
    /* 恢复原状态 */
    set_led(led_num, original_state);
    
    shell_print(sh, "Blinking completed");
    return 0;
}

/* 全部LED控制命令: led_all <on|off|toggle> */
static int cmd_led_all(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: led_all <on|off|toggle>");
        return -EINVAL;
    }

    bool state;
    const char *action = argv[1];

    for (int i = 0; i < NUM_LEDS; i++) {
        if (strcmp(action, "on") == 0) {
            state = true;
        } else if (strcmp(action, "off") == 0) {
            state = false;
        } else if (strcmp(action, "toggle") == 0) {
            state = !led_states[i];
        } else {
            shell_error(sh, "Invalid action. Use: on, off, or toggle");
            return -EINVAL;
        }
        
        set_led(i, state);
    }

    shell_print(sh, "All LEDs turned %s", 
                strcmp(action, "toggle") == 0 ? "toggled" : action);
    return 0;
}

/* 系统信息命令: sysinfo */
static int cmd_sysinfo(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "=== nRF54L15 Button LED Control System ===");
    shell_print(sh, "Board: nRF54L15DK");
    shell_print(sh, "LEDs: %d (GPIO P2.09, P1.10, P2.07, P1.14)", NUM_LEDS);
    shell_print(sh, "Buttons: %d (GPIO P1.13, P1.09, P1.08, P0.04)", NUM_BUTTONS);
    shell_print(sh, "LED Config: GPIO_ACTIVE_HIGH");
    shell_print(sh, "Button Config: GPIO_ACTIVE_LOW with PULL_UP");
    shell_print(sh, "Firmware: Built on " __DATE__ " " __TIME__);
    return 0;
}

/* 重置统计命令: reset_stats */
static int cmd_reset_stats(const struct shell *sh, size_t argc, char **argv)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        button_press_counts[i] = 0;
    }
    shell_print(sh, "Button press statistics reset");
    return 0;
}

/* Shell命令定义 */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_led,
    SHELL_CMD(on, NULL, "Turn on LED: led on <led_num>", cmd_led),
    SHELL_CMD(off, NULL, "Turn off LED: led off <led_num>", cmd_led),
    SHELL_CMD(toggle, NULL, "Toggle LED: led toggle <led_num>", cmd_led),
    SHELL_CMD(status, NULL, "Show LED status", cmd_led_status),
    SHELL_CMD(blink, NULL, "Blink LED: led blink <led_num> <times> [interval_ms]", cmd_led_blink),
    SHELL_CMD(all, NULL, "Control all LEDs: led all <on|off|toggle>", cmd_led_all),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led, &sub_led, "LED control commands", cmd_led);
SHELL_CMD_REGISTER(button_stats, NULL, "Show button press statistics", cmd_button_stats);
SHELL_CMD_REGISTER(sysinfo, NULL, "Show system information", cmd_sysinfo);
SHELL_CMD_REGISTER(reset_stats, NULL, "Reset button press statistics", cmd_reset_stats);
SHELL_CMD_REGISTER(gpio_info, NULL, "Show GPIO information", cmd_gpio_info);
SHELL_CMD_REGISTER(gpio_set, NULL, "Direct GPIO control: gpio_set <led_num> <0|1>", cmd_gpio_set);

/* LED初始化函数 */
int init_leds(void)
{
    int ret;

    for (int i = 0; i < NUM_LEDS; i++) {
        if (!device_is_ready(leds[i].port)) {
            LOG_ERR("LED %d GPIO device not ready", i);
            return -1;
        }

        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            LOG_ERR("Cannot configure LED %d (%d)", i, ret);
            return ret;
        }
        
        /* 确保LED初始状态为关闭 */
        gpio_pin_set_dt(&leds[i], false);
        led_states[i] = false;
        
        LOG_INF("LED %d initialized: Port=%p Pin=%d", i, leds[i].port, leds[i].pin);
    }

    LOG_INF("All LEDs initialized successfully");
    return 0;
}

/* 按键初始化函数 */
int init_buttons(void)
{
    int ret;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (!device_is_ready(buttons[i].port)) {
            LOG_ERR("Button %d GPIO device not ready", i);
            return -1;
        }

        ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
        if (ret < 0) {
            LOG_ERR("Cannot configure button %d (%d)", i, ret);
            return ret;
        }

        ret = gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
        if (ret < 0) {
            LOG_ERR("Cannot configure button %d interrupt (%d)", i, ret);
            return ret;
        }

        gpio_init_callback(&button_cb_data[i], button_pressed, BIT(buttons[i].pin));
        gpio_add_callback(buttons[i].port, &button_cb_data[i]);
        
        LOG_INF("Button %d initialized: Port=%p Pin=%d", i, buttons[i].port, buttons[i].pin);
    }

    LOG_INF("All buttons initialized successfully");
    return 0;
}

/* LED闪烁演示函数 */
void led_demo(void)
{
    LOG_INF("Starting LED demo...");
    
    /* 依次点亮所有LED */
    for (int i = 0; i < NUM_LEDS; i++) {
        set_led(i, true);
        k_msleep(300);
    }
    
    k_msleep(500);
    
    /* 依次关闭所有LED */
    for (int i = 0; i < NUM_LEDS; i++) {
        set_led(i, false);
        k_msleep(300);
    }
    
    LOG_INF("LED demo complete");
}

/* 主函数 */
int main(void)
{
    int ret;

    LOG_INF("=== nRF54L15 Button LED Control with Shell (Fixed) ===");

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

    LOG_INF("System ready!");
    LOG_INF("Hardware controls:");
    LOG_INF("- Button 0 -> LED 0, Button 1 -> LED 1");
    LOG_INF("- Button 2 -> LED 2, Button 3 -> LED 3");
    LOG_INF("");
    LOG_INF("Shell commands available:");
    LOG_INF("- led <on|off|toggle> <led_num>");
    LOG_INF("- led status, led all <on|off|toggle>");
    LOG_INF("- led blink <led_num> <times> [interval_ms]");
    LOG_INF("- gpio_info, gpio_set <led_num> <0|1>");
    LOG_INF("- button_stats, reset_stats, sysinfo");

    /* 主循环 */
    while (1) {
        k_msleep(5000);
        /* 周期性心跳日志 */
        static int heartbeat = 0;
        LOG_DBG("Heartbeat: %d", ++heartbeat);
    }

    return 0;
}