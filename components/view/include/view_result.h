/**
 * @file   view_result.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  验证结果展示视图（View层）声明文件
 *
 * 布局：居中大图标（成功/失败）+ 验证类型文字说明。
 * 支持三种验证方式：人脸、指纹、密码，两种结果：成功、失败。
 * 遵循MVP模式：仅负责UI渲染，Presenter 控制显示时机和后续跳转。
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
 * @brief 验证类型枚举
 */
typedef enum {
    VERIFY_TYPE_FACE = 0,          ///< 人脸验证
    VERIFY_TYPE_FINGERPRINT,       ///< 指纹验证
    VERIFY_TYPE_PASSWORD,          ///< 密码验证
} verify_type_t;

/**
 * @brief 验证结果枚举
 */
typedef enum {
    VERIFY_RESULT_SUCCESS = 0,     ///< 验证成功
    VERIFY_RESULT_FAIL,            ///< 验证失败
} verify_result_t;

/**
 * @brief 验证结果视图操作接口（由 Presenter 调用）
 */
typedef struct {
    /**
     * @brief 显示验证结果
     * 自动加载结果屏幕，根据 type 和 result 渲染对应图标与文字。
     * @param type   验证方式
     * @param result 验证结果（成功/失败）
     */
    void (*show)(verify_type_t type, verify_result_t result);
} view_result_ops_t;

/**
 * @brief 创建验证结果视图（由 View 管理器在初始化时调用）
 * @return 1 成功，0 失败
 */
uint8_t view_result_create(void);

/**
 * @brief 获取验证结果视图操作接口
 * @return view_result_ops_t 指针，未创建返回 NULL
 */
const view_result_ops_t *view_result_get_ops(void);

/**
 * @brief 控制验证结果页面可见性
 * @param visible true 切换到结果页面，false 隐藏
 */
void view_result_set_visible(bool visible);

#ifdef __cplusplus
}
#endif
