/**
 * @file   bsp_touch.h
 * @author xLumina
 * @date   2026-05-07
 * @brief  TOUCH 设备抽象声明文件
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
 * @brief TOUCH 设备初始化函数
 */
uint8_t bsp_touch_init(void);

/**
 * @brief TOUCH 设备反初始化函数
 */
uint8_t bsp_touch_deinit(void);

uint8_t bsp_touch_read_xy(uint16_t *x, uint16_t *y);

uint8_t bsp_touch_swap_xy(bool swap);

uint8_t bsp_touch_mirror_x(bool mirror);

uint8_t bsp_touch_mirror_y(bool mirror);

uint8_t bsp_touch_enter_sleep(void);

uint8_t bsp_touch_exit_sleep(void);

#ifdef __cplusplus
}
#endif