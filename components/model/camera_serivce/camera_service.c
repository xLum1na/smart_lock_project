/**
 * @file   camera_service.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  摄像头数据服务（Model层）实现文件
 *
 * 流水线：摄像头JPEG帧 → JPEG硬件解码 → 480x320 RGB565
 *        → 旋转90度 → 320x480 RGB565 → 输出给调用方
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "camera_service.h"
#include "bsp_camera.h"
#include "bsp_config.h"
#include "esp_camera.h"
#include "jpeg_decoder.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "camera_srv";

/* 摄像头是否已初始化 */
static bool initialized = false;

/**
 * @brief 将 480x320 RGB565 帧旋转90度为 320x480
 * 旋转方式：逆时针90度
 */
static void rotate_480x320_to_320x480(const uint16_t *src,
                                       uint16_t src_w,
                                       uint16_t src_h,
                                       uint16_t *dst)
{
    const uint16_t tgt_w = 320;
    const uint16_t tgt_h = 480;

    for (int y = 0; y < tgt_h; y++) {
        for (int x = 0; x < tgt_w; x++) {
            int sx = y;
            int sy = src_h - 1 - x;
            dst[y * tgt_w + x] = src[sy * src_w + sx];
        }
    }
}

uint8_t camera_service_init(void)
{
    if (initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return 1;
    }

    if (bsp_camera_init() != 1) {
        ESP_LOGE(TAG, "BSP camera init failed");
        return 0;
    }

    initialized = true;
    ESP_LOGI(TAG, "Camera service initialized");
    return 1;
}

uint8_t camera_service_deinit(void)
{
    if (!initialized) {
        return 1;
    }
    initialized = false;
    return bsp_camera_deinit();
}

uint8_t camera_service_get_frame(camera_frame_t *out_frame)
{
    if (!initialized) {
        ESP_LOGE(TAG, "Camera not initialized");
        return 0;
    }
    if (!out_frame) {
        return 0;
    }
    memset(out_frame, 0, sizeof(camera_frame_t));

    /* 第一步：从摄像头获取 JPEG 帧 */
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "esp_camera_fb_get failed");
        return 0;
    }

    /* 第二步：获取 JPEG 图像信息 */
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = fb->buf,
        .indata_size = fb->len,
        .outbuf = NULL,
        .outbuf_size = 0,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags.swap_color_bytes = 0,
    };

    esp_jpeg_image_output_t jpeg_out;
    esp_err_t err = esp_jpeg_get_image_info(&jpeg_cfg, &jpeg_out);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "JPEG info failed: %s", esp_err_to_name(err));
        esp_camera_fb_return(fb);
        return 0;
    }

    /* 第三步：分配解码缓冲并执行解码 */
    size_t decoded_size = jpeg_out.output_len;
    uint16_t *decoded_buf = heap_caps_malloc(decoded_size,
                                              MALLOC_CAP_SPIRAM);
    if (!decoded_buf) {
        ESP_LOGE(TAG, "Decode buffer alloc failed");
        esp_camera_fb_return(fb);
        return 0;
    }

    jpeg_cfg.outbuf = (uint8_t *)decoded_buf;
    jpeg_cfg.outbuf_size = decoded_size;
    err = esp_jpeg_decode(&jpeg_cfg, &jpeg_out);
    esp_camera_fb_return(fb); // 尽早归还摄像头帧缓冲

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "JPEG decode failed: %s", esp_err_to_name(err));
        heap_caps_free(decoded_buf);
        return 0;
    }

    /* 第四步：分配旋转目标缓冲并执行旋转 */
    size_t rotated_size = CAMERA_FRAME_WIDTH * CAMERA_FRAME_HEIGHT
                          * sizeof(uint16_t);
    uint16_t *rotated_buf = heap_caps_malloc(rotated_size,
                                              MALLOC_CAP_SPIRAM);
    if (!rotated_buf) {
        ESP_LOGE(TAG, "Rotate buffer alloc failed");
        heap_caps_free(decoded_buf);
        return 0;
    }

    rotate_480x320_to_320x480(decoded_buf,
                               jpeg_out.width, jpeg_out.height,
                               rotated_buf);
    heap_caps_free(decoded_buf);

    /* 第五步：填充输出结构 */
    out_frame->data = rotated_buf;
    out_frame->width = CAMERA_FRAME_WIDTH;
    out_frame->height = CAMERA_FRAME_HEIGHT;

    return 1;
}

void camera_service_release_frame(camera_frame_t *frame)
{
    if (frame && frame->data) {
        heap_caps_free(frame->data);
        frame->data = NULL;
    }
}