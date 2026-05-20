/**
 * @file   camera_service.h
 * @author xLumina
 * @date   2026-05-20
 * @brief  摄像头采集服务 - 双缓冲 + 信号量
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void camera_service_start(void);
void camera_service_stop(void);

/** 获取最新就绪帧 (非阻塞, 无新帧返回 NULL) */
uint8_t *camera_service_get_frame(void);
void     camera_service_get_resolution(uint16_t *w, uint16_t *h);

#ifdef __cplusplus
}
#endif