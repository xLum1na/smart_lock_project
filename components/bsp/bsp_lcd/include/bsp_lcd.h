/**
 * @file   bsp_lcd.h
 * @author xLumina
 * @date   2026-05-07
 * @brief  lcd 设备抽象声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 初始化 LCD，包括硬件复位、发送初始化序列并打开显示
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_init(void);

/**
 * @brief 销毁 LCD 驱动，释放资源
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_deinit(void);

/**
 * @brief 打开或关闭显示（不影响背光）
 * @param on true 打开，false 关闭
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_display_on_off(bool on);

/**
 * @brief 进入或退出休眠模式
 * @param sleep true 进入休眠，false 唤醒
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_sleep(bool sleep);

/**
 * @brief 设置屏幕镜像
 * @param x 水平镜像
 * @param y 垂直镜像
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_mirror(bool x, bool y);

/**
 * @brief 交换 XY 轴（配合镜像可实现旋转）
 * @param swap true 交换，false 正常
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_swap_xy(bool swap);

/**
 * @brief 设置显示偏移（用于居中等比屏幕小的内容）
 * @param x_gap X 方向偏移（像素）
 * @param y_gap Y 方向偏移（像素）
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_set_gap(int x_gap, int y_gap);

/**
 * @brief 反转颜色（按位取反）
 * @param invert true 反转，false 正常
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_invert_color(bool invert);

/**
 * @brief 在指定矩形区域绘制位图
 * @param x 起始列
 * @param y 起始行
 * @param x0 结束列
 * @param y0 结束行
 * @param pixels 像素数据，RGB565 格式
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_draw_bitmap(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0,
                        const void *pixels);

/**
 * @brief 获取屏幕宽度
 * @return 宽度（像素）
 */
uint8_t bsp_lcd_get_width(void);

/**
 * @brief 获取屏幕高度
 * @return 高度（像素）
 */
uint8_t bsp_lcd_get_height(void);

/**
 * @brief 设置背光亮度
 * @param percent 0 ~ 100
 * @return 0 成功，-1 失败
 */
uint8_t bsp_lcd_set_backlight(uint8_t percent);

#ifdef __cplusplus
}
#endif