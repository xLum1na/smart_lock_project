/**
 * @file   presenter.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  Presenter 层公共声明文件
 *
 * Presenter 层是 MVP 架构的协调核心，持有 Model 和 View 引用，
 * 负责将 Model 数据转换为 View 可显示的格式。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化所有 Presenter 模块
 *
 * 应在 view_init() 之后调用，内部按依赖顺序初始化各子模块。
 * @return 1 成功，0 失败
 */
uint8_t presenter_init(void);

#ifdef __cplusplus
}
#endif