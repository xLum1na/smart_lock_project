/**
 * @file   camera_service.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  摄像头数据服务（Model层）声明文件
 *
 * 职责：封装摄像头硬件操作、JPEG解码、图像旋转，
 *       对外输出 320x480 RGB565 帧数据。
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

/* 输出帧尺寸常量 */
#define CAMERA_FRAME_WIDTH  320
#define CAMERA_FRAME_HEIGHT 480

/**
 * @brief 摄像头帧数据结构
 */
typedef struct {
    uint16_t *data;         ///< RGB565 像素数据指针
    uint16_t width;         ///< 帧宽度（320）
    uint16_t height;        ///< 帧高度（480）
} camera_frame_t;

/**
 * @brief 初始化摄像头硬件，配置 JPEG 输出模式
 * @return 1 成功，0 失败
 */
uint8_t camera_service_init(void);

/**
 * @brief 反初始化摄像头，释放硬件资源
 * @return 1 成功，0 失败
 */
uint8_t camera_service_deinit(void);

/**
 * @brief 抓取一帧，完成JPEG解码和旋转，输出 320x480 RGB565
 *
 * 从摄像头获取JPEG帧 → 硬件解码为RGB565 → 旋转90度适配竖屏
 * → 填充到调用方提供的帧结构中。
 *
 * @param[out] out_frame 输出帧结构，调用方需通过 camera_service_release_frame 释放
 * @return 1 成功，0 失败
 */
uint8_t camera_service_get_frame(camera_frame_t *out_frame);

/**
 * @brief 释放帧数据缓冲区
 * @param frame 待释放的帧结构
 */
void camera_service_release_frame(camera_frame_t *frame);

#ifdef __cplusplus
}
#endif