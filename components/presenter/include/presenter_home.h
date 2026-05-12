/**
 * @file   presenter_home.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  主页面协调器（Presenter层）声明文件
 *
 * 职责：持有 Model（camera_service）和 View（view_home）的引用，
 *       协调摄像头帧抓取→视图刷新的完整流程。
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
 * @brief 初始化主页面 Presenter
 * 内部会初始化摄像头服务和主页面视图。
 * @return 1 成功，0 失败
 */
uint8_t presenter_home_init(void);

/**
 * @brief 执行一帧摄像头预览更新
 * 抓取摄像头帧 → 更新主页面视图显示
 * @return 1 成功，0 失败
 */
uint8_t presenter_home_tick(void);

#ifdef __cplusplus
}
#endif