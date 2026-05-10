/**
 * @file   bsp_config.h
 * @author xLumina
 * @date   2026-05-05
 * @brief  板级配置文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-05 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 板上SPI资源配置定义 */
#define BOARD_MOSI_PIN    12      ///< SPI 主机输出从机输入 IO
#define BOARD_MISO_PIN    13      ///< SPI 主机输入从机输出 IO
#define BOARD_SCLK_PIN    14      ///< SPI 时钟 IO

/* 板上显示设备资源配置定义 */
#define DISPLAY_CS_PIN      15      ///< 显示设备 SPI 片选 IO
#define DISPLAY_LEDC_PIN    16      ///< 显示设备 LED IO
#define DISPLAY_DC_PIN      17      ///< 显示设备数据/命令 IO
#define DISPLAY_RESET_PIN   18      ///< 显示设备复位 IO






#ifdef __cplusplus
}
#endif