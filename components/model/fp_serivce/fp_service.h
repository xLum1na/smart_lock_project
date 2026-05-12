/**
 * @file   fp_service.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  指纹数据服务（Model层）声明文件
 *
 * 封装 bsp_fp 硬件操作，提供指纹录入、删除、验证功能。
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

/* 指纹 ID 范围 0x01 ~ 0x80 */
#define FP_ID_MIN  1
#define FP_ID_MAX  128

/**
 * @brief 初始化指纹模块，设置工作模式并开启识别
 * @return 1 成功，0 失败
 */
uint8_t fp_service_init(void);

/**
 * @brief 反初始化指纹模块
 * @return 1 成功，0 失败
 */
uint8_t fp_service_deinit(void);

/**
 * @brief 录入一枚指纹（阻塞，约 25~30 秒）
 * @param id 指纹 ID（1~128）
 * @return 1 录入成功，0 失败
 */
uint8_t fp_service_enroll(uint8_t id);

/**
 * @brief 删除指定 ID 的指纹
 * @param id 指纹 ID（1~128）
 * @return 1 删除成功，0 失败
 */
uint8_t fp_service_delete(uint8_t id);

/**
 * @brief 删除全部指纹
 * @return 1 成功，0 失败
 */
uint8_t fp_service_delete_all(void);

/**
 * @brief 等待指纹验证（阻塞）
 * @param[out] matched_id 匹配成功时返回指纹 ID
 * @param timeout_ms 超时时间（毫秒），0 表示永不超时
 * @return 1 验证成功（*matched_id 有效），0 超时或失败
 */
uint8_t fp_service_verify(uint8_t *matched_id, uint32_t timeout_ms);

/**
 * @brief 取消当前指纹验证（使阻塞的 verify 调用快速返回）
 */
void fp_service_cancel_verify(void);

#ifdef __cplusplus
}
#endif
