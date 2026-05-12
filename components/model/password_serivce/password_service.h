/**
 * @file   password_service.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  密码数据服务（Model层）声明文件
 *
 * 使用 NVS 持久化存储管理员密码与开锁密码，提供设置与验证接口。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 密码最大长度（不含结束符） */
#define PASSWORD_MAX_LEN  6

/**
 * @brief 初始化密码服务（打开 NVS 命名空间）
 * @return 1 成功，0 失败
 */
uint8_t password_service_init(void);

/**
 * @brief 设置管理员密码
 * @param pwd 新密码（6位数字字符串，如 "123456"）
 * @return 1 成功，0 失败
 */
uint8_t password_service_set_admin(const char *pwd);

/**
 * @brief 设置开锁密码
 * @param pwd 新密码（6位数字字符串）
 * @return 1 成功，0 失败
 */
uint8_t password_service_set_unlock(const char *pwd);

/**
 * @brief 验证管理员密码
 * @param pwd 待验证的密码
 * @return 1 密码正确，0 密码错误或未设置
 */
uint8_t password_service_verify_admin(const char *pwd);

/**
 * @brief 验证开锁密码
 * @param pwd 待验证的密码
 * @return 1 密码正确，0 密码错误或未设置
 */
uint8_t password_service_verify_unlock(const char *pwd);

/**
 * @brief 检查管理员密码是否已设置
 * @return true 已设置，false 未设置
 */
bool password_service_admin_is_set(void);

/**
 * @brief 检查开锁密码是否已设置
 * @return true 已设置，false 未设置
 */
bool password_service_unlock_is_set(void);

#ifdef __cplusplus
}
#endif