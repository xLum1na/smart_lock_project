/**
 * @file   display_service.c
 * @author xLumina
 * @date   2026-05-20
 * @brief  LCD 显示服务 - 从 camera_service 取帧, 拷贝到自有缓冲, 刷屏
 */

#include "display_service.h"
#include "camera_service.h"
#include "bsp_lcd.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include <string.h>

static const char *TAG = "disp_srv";

static uint8_t  *disp_buf = NULL;   /* 自有缓冲, 消费者独占 */
static uint32_t  frame_size = 0;
static uint16_t  w = 0, h = 0;

void display_service_init(void)
{
    camera_service_get_resolution(&w, &h);
    frame_size = w * h * 2;

    disp_buf = heap_caps_malloc(frame_size, MALLOC_CAP_SPIRAM);
    if (!disp_buf) {
        ESP_LOGE(TAG, "缓冲区分配失败");
        return;
    }

    /* 首帧填充 (camera_service_start 后已有帧就绪) */
    uint8_t *first = camera_service_get_frame();
    if (first) {
        memcpy(disp_buf, first, frame_size);
        bsp_lcd_draw_bitmap(0, 0, w, h, disp_buf);
        ESP_LOGI(TAG, "首帧显示完成");
    }

    ESP_LOGI(TAG, "显示服务初始化 %d×%d", w, h);
}

void display_service_update(void)
{
    uint8_t *frame = camera_service_get_frame();
    if (frame) {
        memcpy(disp_buf, frame, frame_size);
        bsp_lcd_draw_bitmap(0, 0, w, h, disp_buf);
    }
}

void display_service_deinit(void)
{
    if (disp_buf) {
        heap_caps_free(disp_buf);
        disp_buf = NULL;
    }
    ESP_LOGI(TAG, "显示服务已销毁");
}