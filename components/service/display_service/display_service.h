/**
 * @file   display_service.h
 * @author xLumina
 * @date   2026-05-20
 * @brief  LCD 显示服务 - 消费 camera_service 画面
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void display_service_init(void);
void display_service_update(void);
void display_service_deinit(void);

#ifdef __cplusplus
}
#endif