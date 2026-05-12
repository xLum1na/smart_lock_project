/**
 * @file   view_admin.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  管理员设置视图（View层）声明文件
 *
 * 布局：顶部标题 + 可滚动功能菜单列表（5项）。
 * 菜单项：人脸设置、指纹设置、管理员密码设置、开锁密码设置、日志显示。
 * 遵循MVP模式：菜单点击通过回调通知Presenter。
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
 * @brief 管理员菜单项枚举
 */
typedef enum {
    ADMIN_MENU_FACE_SET = 0,       ///< 人脸设置（注册/删除）
    ADMIN_MENU_FP_SET,             ///< 指纹设置（注册/删除）
    ADMIN_MENU_ADMIN_PWD,          ///< 管理员密码设置
    ADMIN_MENU_UNLOCK_PWD,         ///< 开锁密码设置
    ADMIN_MENU_LOG,                ///< 日志显示
    ADMIN_MENU_MAX                 ///< 菜单项总数（5）
} admin_menu_item_t;

/**
 * @brief 管理员菜单点击回调类型
 * @param item 被点击的菜单项
 * @param arg  用户注册的上下文参数
 */
typedef void (*admin_menu_cb_t)(admin_menu_item_t item, void *arg);

/**
 * @brief 管理员视图操作接口（由 Presenter 调用）
 */
typedef struct {
    /**
     * @brief 注册菜单点击回调
     * @param cb        回调函数
     * @param user_data 回调上下文参数
     */
    void (*set_menu_callback)(admin_menu_cb_t cb, void *user_data);
} view_admin_ops_t;

/**
 * @brief 创建管理员设置视图（由 View 管理器在初始化时调用）
 * @return 1 成功，0 失败
 */
uint8_t view_admin_create(void);

/**
 * @brief 获取管理员视图操作接口
 * @return view_admin_ops_t 指针，未创建返回 NULL
 */
const view_admin_ops_t *view_admin_get_ops(void);

/**
 * @brief 控制管理员页面屏幕可见性
 * @param visible true 切换到管理员页面，false 隐藏
 */
void view_admin_set_visible(bool visible);

#ifdef __cplusplus
}
#endif
