/**
 * @file   view_password.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  密码输入视图（View层）声明文件
 *
 * 布局：上部为密码掩码显示区域（6位●），下部为数字键盘（0-9、退格、确认）。
 * 遵循MVP模式：本模块仅负责UI渲染，按键事件通过回调通知Presenter。
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

/* 密码最大位数 */
#define PASSWORD_MAX_LEN  6

/**
 * @brief 键盘按键类型枚举
 */
typedef enum {
    PASSWORD_KEY_0 = '0',
    PASSWORD_KEY_1 = '1',
    PASSWORD_KEY_2 = '2',
    PASSWORD_KEY_3 = '3',
    PASSWORD_KEY_4 = '4',
    PASSWORD_KEY_5 = '5',
    PASSWORD_KEY_6 = '6',
    PASSWORD_KEY_7 = '7',
    PASSWORD_KEY_8 = '8',
    PASSWORD_KEY_9 = '9',
    PASSWORD_KEY_BACKSPACE = 'B',   ///< 退格/删除
    PASSWORD_KEY_CONFIRM   = 'C',   ///< 确认/确定
} password_key_t;

/**
 * @brief 按键回调类型，当用户点击键盘按钮时触发
 * @param key  被按下的按键字符（参见 password_key_t）
 * @param arg  用户注册的上下文参数
 */
typedef void (*password_key_cb_t)(char key, void *arg);

/**
 * @brief 密码输入视图操作接口（由 Presenter 调用）
 */
typedef struct {
    /**
     * @brief 更新密码掩码显示数量
     * @param count 当前已输入位数（0~6），内部渲染对应数量的●
     */
    void (*update_display)(uint8_t count);

    /**
     * @brief 清空密码显示（恢复全空状态）
     */
    void (*clear_display)(void);

    /**
     * @brief 注册按键回调函数
     * @param cb        按键回调，用户点击键盘时触发
     * @param user_data 回调上下文，透传给回调函数
     */
    void (*set_key_callback)(password_key_cb_t cb, void *user_data);
} view_password_ops_t;

/**
 * @brief 创建密码输入视图（由 View 管理器在初始化时调用）
 * @return 1 成功，0 失败
 */
uint8_t view_password_create(void);

/**
 * @brief 获取密码输入视图操作接口，供 Presenter 持有
 * @return view_password_ops_t 指针，页面未创建时返回 NULL
 */
const view_password_ops_t *view_password_get_ops(void);

/**
 * @brief 控制密码输入页面屏幕可见性
 * @param visible true 切换到密码页面，false 隐藏
 */
void view_password_set_visible(bool visible);

#ifdef __cplusplus
}
#endif
