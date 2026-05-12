/**
 * @file   view_home.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  主页面视图（View层）实现文件
 *
 * 纯UI渲染：LVGL全屏图像控件，通过 ops 接口接收 Presenter 数据。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "view_home.h"
#include "lvgl.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "view_home";

static lv_obj_t *home_screen = NULL;
static lv_obj_t *camera_img = NULL;
static uint16_t *frame_buffer = NULL;
static lv_image_dsc_t camera_img_dsc;
static bool is_created = false;

/* ---- 视图操作接口实现 ---- */
static uint8_t home_update_frame(const uint16_t *frame,
                                  uint16_t width, uint16_t height);

static view_home_ops_t home_ops = {
    .update_frame = home_update_frame,
};

static uint8_t home_update_frame(const uint16_t *frame,
                                  uint16_t width, uint16_t height)
{
    if (!is_created || !frame_buffer || !camera_img) {
        return 0;
    }
    if (width != 320 || height != 480) {
        ESP_LOGW(TAG, "Size mismatch: %dx%d, expected 320x480", width, height);
        return 0;
    }

    memcpy(frame_buffer, frame, width * height * sizeof(uint16_t));
    lv_obj_invalidate(camera_img);
    return 1;
}

uint8_t view_home_create(void)
{
    if (is_created) {
        ESP_LOGW(TAG, "Already created");
        return 1;
    }

    /* 创建主页屏幕 */
    home_screen = lv_obj_create(NULL);
    if (!home_screen) {
        ESP_LOGE(TAG, "Screen create failed");
        return 0;
    }
    lv_obj_set_style_bg_color(home_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(home_screen, LV_OPA_COVER, 0);

    /* 创建全屏图像控件 */
    camera_img = lv_image_create(home_screen);
    if (!camera_img) {
        ESP_LOGE(TAG, "Image create failed");
        return 0;
    }
    lv_obj_set_size(camera_img, 320, 480);
    lv_obj_set_pos(camera_img, 0, 0);

    /* 分配帧缓冲 (SPIRAM) */
    frame_buffer = heap_caps_malloc(320 * 480 * sizeof(uint16_t),
                                     MALLOC_CAP_SPIRAM);
    if (!frame_buffer) {
        ESP_LOGE(TAG, "Frame buffer alloc failed");
        return 0;
    }
    memset(frame_buffer, 0x00, 320 * 480 * sizeof(uint16_t));

    /* 绑定图像描述符 */
    memset(&camera_img_dsc, 0, sizeof(camera_img_dsc));
    camera_img_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    camera_img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    camera_img_dsc.header.w = 320;
    camera_img_dsc.header.h = 480;
    camera_img_dsc.header.stride = 320 * 2;
    camera_img_dsc.data = (const uint8_t *)frame_buffer;
    camera_img_dsc.data_size = 320 * 480 * 2;
    lv_image_set_src(camera_img, &camera_img_dsc);

    is_created = true;
    ESP_LOGI(TAG, "Home view created, 320x480 RGB565");
    return 1;
}

const view_home_ops_t *view_home_get_ops(void)
{
    if (!is_created) {
        return NULL;
    }
    return &home_ops;
}

void view_home_set_visible(bool visible)
{
    if (!is_created || !home_screen) {
        return;
    }
    if (visible) {
        lv_screen_load(home_screen);
    }
}