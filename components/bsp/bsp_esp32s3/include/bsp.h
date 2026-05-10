/**
 * @file   bsp.h
 * @author xLumina
 * @date   2026-05-06
 * @brief  bsp声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-06 | xLumina | 初始版本创建
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
 * @brief 板级初始化函数
 */
void bsp_init(void);


#ifdef __cplusplus
}
#endif