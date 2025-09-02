/* include/uicr.h */
#ifndef UICR_H
#define UICR_H

#include <stdint.h>
#include <stdbool.h>

#define UICR_STR_MAX_LEN 13  // 12字符 + null终止符

/**
 * @brief 获取设备序列号
 * @return 序列号字符串指针
 */
char *uicr_serial_number_get(void);

/**
 * @brief 获取出厂标志位
 * @return true表示已出厂，false表示开发模式
 */
bool uicr_shipping_flag_get(void);

/**
 * @brief 获取WiFi MAC地址（简化版）
 * @return MAC地址字符串
 */
char *uicr_wifi_mac_address_get(void);

#endif /* UICR_H */