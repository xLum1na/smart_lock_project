/**
 * @file   bsp_gpio.h
 * @author xLumina
 * @date   2026-05-07
 * @brief  GPIO 外设初始化声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
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
 * @brief GPIO 外设初始化函数
 */
uint8_t bsp_gpio_init(void);

/**
 * @brief GPIO 外设反初始化函数
 */
uint8_t bsp_gpio_deinit(void);

#ifdef __cplusplus
}
#endif