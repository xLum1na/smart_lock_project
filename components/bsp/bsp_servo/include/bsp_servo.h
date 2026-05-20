/**
 * @file   bsp_servo.h
 * @author xLumina
 * @date   2026-05-18
 * @brief  舵机 设备驱动声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-18 | xLumina | 初始版本创建
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
 * @brief 舵机 设备初始化函数
 */
uint8_t bsp_servo_init(void);

/**
 * @brief 舵机 设备反初始化函数
 */
uint8_t bsp_servo_deinit(void);

/**
 * @brief 舵机 设置角度函数
 */
uint8_t bsp_servo_set_angle(uint16_t angle);


#ifdef __cplusplus
}
#endif