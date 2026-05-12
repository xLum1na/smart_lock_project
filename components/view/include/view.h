/**
 * @file   view.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  UI视图管理器（View层）声明文件
 *
 * 负责LVGL显示驱动注册、视图页面生命周期管理和页面切换。
 * 遵循MVP模式：本层仅负责UI渲染，不包含业务逻辑。
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
 * @brief 视图ID枚举
 */
typedef enum {
    VIEW_ID_NONE = -1,      ///< 无效视图
    VIEW_ID_HOME = 0,       ///< 主页面 - 全屏摄像头预览
    VIEW_ID_PASSWORD,       ///< 密码输入页面 - 密码掩码显示 + 数字键盘
    VIEW_ID_RESULT,         ///< 验证结果页面 - 成功/失败图标与文字
    VIEW_ID_ADMIN,          ///< 管理员设置页面 - 7项功能菜单列表
    VIEW_ID_MAX             ///< 视图总数上限
} view_id_t;

/**
 * @brief 初始化视图管理系统（LVGL显示驱动、帧缓冲、定时器）
 * 同时创建各子视图页面，默认进入 VIEW_ID_HOME。
 * @return 1 成功，0 失败
 */
uint8_t view_init(void);

/**
 * @brief 切换到指定视图页面
 * @param id 目标视图ID
 * @return 1 成功，0 失败
 */
uint8_t view_switch_to(view_id_t id);

/**
 * @brief 获取当前活跃视图ID
 * @return 当前视图ID
 */
view_id_t view_get_current(void);

/**
 * @brief LVGL 任务处理循环，需作为独立 FreeRTOS 任务周期性运行
 * @param arg 未使用
 */
void view_task(void *arg);

#ifdef __cplusplus
}
#endif