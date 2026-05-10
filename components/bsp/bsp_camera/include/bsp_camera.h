/**
 * @file   bsp_camera.h
 * @author xLumina
 * @date   2026-05-10
 * @brief  CAMERA 设备抽象声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-10 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief CAMERA 设备初始化函数
 */
uint8_t bsp_camera_init(void);

/**
 * @brief CAMERA 设备反初始化函数
 */
uint8_t bsp_camera_deinit(void);

#ifdef __cplusplus
}
#endif