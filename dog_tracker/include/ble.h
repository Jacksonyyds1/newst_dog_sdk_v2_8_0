/* include/ble.h */
#ifndef BLE_H
#define BLE_H

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

/**
 * @brief 初始化BLE模块
 * @return 0成功，负值失败
 */
int ble_init(void);

/**
 * @brief 开始BLE广播
 * @param timeout_sec 广播超时时间（秒），0表示不超时
 * @return 0成功，负值失败
 */
int ble_advertise_start(int timeout_sec);

/**
 * @brief 停止BLE广播
 * @return 0成功，负值失败
 */
int ble_stop(void);

/**
 * @brief 获取当前BLE连接
 * @return 连接对象指针，NULL表示无连接
 */
struct bt_conn *ble_get_conn(void);

/**
 * @brief 获取本地设备名称
 * @return 设备名称字符串
 */
char *ble_get_local_name(void);

/**
 * @brief 断开BLE连接
 * @return 0成功，负值失败
 */
int ble_disconnect(void);

/**
 * @brief 关闭BLE功能
 * @return 0成功，负值失败
 */
int ble_shutdown(void);

#endif /* BLE_H */