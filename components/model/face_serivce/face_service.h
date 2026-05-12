/**
 * @file   face_service.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  人脸识别数据服务（Model层）声明文件
 *
 * 提供人脸录入、删除、验证功能。
 * 当前为接口预留实现，具体算法依赖 ESP-WHO 或外部模块。
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

/* 人脸 ID 范围 */
#define FACE_ID_MIN  1
#define FACE_ID_MAX  32

uint8_t face_service_init(void);
uint8_t face_service_deinit(void);
uint8_t face_service_enroll(uint8_t id);
uint8_t face_service_delete(uint8_t id);
uint8_t face_service_delete_all(void);
uint8_t face_service_verify(uint8_t *matched_id);

#ifdef __cplusplus
}
#endif