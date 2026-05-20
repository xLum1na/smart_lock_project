/**
 * @file   bsp_config.h
 * @author xLumina
 * @date   2026-05-06
 * @brief  bsp配置文件
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

/* SPI 总线配置定义 */
#define BSP_SPI_HOST            SPI2_HOST           ///< SPI2
#define BSP_SPI_DMA_CHAN        SPI_DMA_CH_AUTO     ///< 自动选择通道
#define BSP_SPI_TRAN_MAX_SIZE   (480 * 320 * 3)     /* 460800, 容纳全屏 RGB666 */
#define BSP_SPI_MOSI_PIN        38
#define BSP_SPI_MISO_PIN        39
#define BSP_SPI_SCLK_PIN        40

/**
 * @brief 显示设备配置定义
 */
/* 显示设备基础配置定义 */
#define BSP_DISPLAY_WIDTH_PX      320
#define BSP_DISPLAY_HIGHT_PX      480
#define BSP_DISPLAY_DMA_SIZE      320*100*2


/* 显示设备 SPI 配置*/
#define BSP_DISPLAY_CS_PIN          0
#define BSP_DISPLAY_SPI_MODE        0
#define BSP_DISPLAY_SPI_FREQ_HZ     40000000
/* 显示设备 GPIO 配置*/
#define BSP_DISPLAY_DC_PIN          2
#define BSP_DISPLAY_RESET_PIN       1


/* 显示设备 LEDC 配置*/
#define BSP_LEDC_TIMER          LEDC_TIMER_0            ///< timer0
#define BSP_LEDC_MODE           LEDC_LOW_SPEED_MODE     ///< 低速模式
#define BSP_LEDC_CLK            LEDC_AUTO_CLK           ///< 自动选择
#define BSP_LEDC_DUTY_RES       LEDC_TIMER_12_BIT       ///< 12位分辨率
#define BSP_LEDC_FREQ_HZ        4000                    ///< 4 Khz
#define BSP_LEDC_CHANNEL        LEDC_CHANNEL_0          ///< 通道0
#define BSP_DISPLAY_LEDC_PIN    41


/**
 * @brief 触摸设备配置定义
 */
/* 触摸设备 SPI 配置*/
#define BSP_TOUCH_CS_PIN        42
#define BSP_TOUCH_SPI_MODE      0
#define BSP_TOUCH_FREQ_HZ       1000000

/**
 * @brief CAMERA 设备配置定义
 */
/* I2S 引脚定义 */
#define BSP_CAMERA_PWDN_PIN     3
#define BSP_CAMERA_RESET_PIN    4
#define BSP_CAMERA_XCLK_PIN     5
#define BSP_CAMERA_SIOD_PIN     6
#define BSP_CAMERA_SIOC_PIN     7
#define BSP_CAMERA_VSYNC_PIN    8
#define BSP_CAMERA_HREF_PIN     9
#define BSP_CAMERA_PCLK_PIN     10


#define BSP_CAMERA_DATA0_PIN    11
#define BSP_CAMERA_DATA1_PIN    12
#define BSP_CAMERA_DATA2_PIN    13
#define BSP_CAMERA_DATA3_PIN    14
#define BSP_CAMERA_DATA4_PIN    15
#define BSP_CAMERA_DATA5_PIN    16
#define BSP_CAMERA_DATA6_PIN    17
#define BSP_CAMERA_DATA7_PIN    18



#define BSP_CAMERA_XCLK_FREQ_HZ     20000000  /* 20MHz, OV2640 安全范围 */
#define BSP_CAMERA_LEDC_TIMER       LEDC_TIMER_1
#define BSP_CAMERA_LEDC_CHANNEL     LEDC_CHANNEL_0

/**
 * @brief 指纹模块配置定义
 */
#define BSP_FP_UART_PORT    UART_NUM_2
#define BSP_FP_UART_RX_PIN  20
#define BSP_FP_UART_TX_PIN  19

#define BSP_FP_UART_RATE    9600

/**
 * @brief SD模块配置定义
 */
#define BSP_SD_CS_PIN       45

/**
 * @brief 舵机配置定义
 */
#define BSP_SERVO_LEDC_PIIN       48
#define BSP_SERVO_LEDC_TIMER    LEDC_TIMER_2
#define BSP_SERVO_LEDC_CHANNEL  LEDC_CHANNEL_0
#define BSP_SERVO_MIN_PW_US     500
#define BSP_SERVO_MAX_PW_US     2500

#ifdef __cplusplus
}
#endif