/**
 * @file   view_home.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  主页面视图（View层）声明文件
 *
 * 定义 Presenter 可调用的视图操作接口，遵循MVP模式。
 * 本模块仅负责 LVGL 控件管理和屏幕渲染，不含业务逻辑。
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

/**
 * @brief 主页面视图操作接口（由 Presenter 调用）
 */
typedef struct {
    /**
     * @brief 更新摄像头预览帧
     * @param frame  RGB565 像素数据（320x480）
     * @param width  帧宽度（320）
     * @param height 帧高度（480）
     * @return 1 成功，0 失败
     */
    uint8_t (*update_frame)(const uint16_t *frame, uint16_t width, uint16_t height);
} view_home_ops_t;

/**
 * @brief 创建主页面视图（由 View 管理器在初始化时调用）
 * @return 1 成功，0 失败
 */
uint8_t view_home_create(void);

/**
 * @brief 获取主页面视图操作接口，供 Presenter 持有
 * @return view_home_ops_t 指针，页面未创建时返回 NULL
 */
const view_home_ops_t *view_home_get_ops(void);

/**
 * @brief 控制主页面屏幕可见性
 * @param visible true 切换到主页屏幕，false 隐藏
 */
void view_home_set_visible(bool visible);

#ifdef __cplusplus
}
#endif