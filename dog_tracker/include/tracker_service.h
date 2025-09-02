/* include/tracker_service.h */
#ifndef TRACKER_SERVICE_H
#define TRACKER_SERVICE_H

#include <zephyr/bluetooth/uuid.h>

/* Tracker Service UUID: 9d1589a6-cea6-4df1-96d9-1697cd4dc1e7 */
#define BT_UUID_TRACKER_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x9d1589a6, 0xcea6, 0x4df1, 0x96d9, 0x1697cd4dc1e7)

/**
 * @brief 初始化Tracker BLE服务
 * @return 0成功，负值失败
 */
int tracker_service_init(void);

/**
 * @brief 检查设备是否已配对
 * @return true已配对，false未配对
 */
bool tracker_service_is_onboarded(void);

/**
 * @brief 发送FOTA状态通知
 * @param fota_state FOTA状态字符串
 * @return 0成功，负值失败
 */
int fota_notify(char *fota_state);

#endif /* TRACKER_SERVICE_H */