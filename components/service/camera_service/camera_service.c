/**
 * @file   camera_service.c
 * @author xLumina
 * @date   2026-05-20
 * @brief  摄像头采集服务 - 取帧 / 字节交换 / 双缓冲 / 信号量
 */

#include "camera_service.h"
#include "bsp_camera.h"

#include "esp_camera.h"
// #include "esp_cache.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <string.h>

static const char *TAG = "cam_srv";

static uint8_t  *buf[2] = {NULL, NULL};
static uint8_t   write_idx = 0;
static uint8_t  *ready_buf = NULL;
static uint32_t  frame_size = 0;
static uint16_t  frame_w = 0, frame_h = 0;

static SemaphoreHandle_t sem = NULL;
static TaskHandle_t task = NULL;
static volatile bool running = false;

/* ==========================================================================
 * 字节交换
 * ========================================================================== */
static void swap_bytes(uint8_t *p, uint32_t len)
{
    for (uint32_t i = 0; i < len; i += 2) {
        uint8_t t = p[i]; p[i] = p[i+1]; p[i+1] = t;
    }
}

/* ==========================================================================
 * 采集任务
 * ========================================================================== */
static void capture_task(void *arg)
{
    while (running) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) { vTaskDelay(pdMS_TO_TICKS(10)); continue; }

        // esp_cache_msync((void *)fb->buf, fb->len,
        //                 ESP_CACHE_MSYNC_FLAG_INVALIDATE |
        //                 ESP_CACHE_MSYNC_FLAG_UNALIGNED);

        /* 拷贝到写缓冲 → 归还摄像头帧 → 字节交换 */
        memcpy(buf[write_idx], fb->buf, frame_size);
        esp_camera_fb_return(fb);
        swap_bytes(buf[write_idx], frame_size);

        /* 发布就绪帧 */
        ready_buf = buf[write_idx];
        write_idx = 1 - write_idx;
        xSemaphoreGive(sem);

        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

/* ==========================================================================
 * 公开接口
 * ========================================================================== */
void camera_service_start(void)
{
    if (running) return;

    /* 首帧获取尺寸 */
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) { ESP_LOGE(TAG, "首帧失败"); return; }
    frame_w  = (uint16_t)fb->width;
    frame_h  = (uint16_t)fb->height;
    frame_size = frame_w * frame_h * 2;
    ESP_LOGI(TAG, "摄像头 %d×%d, %d 字节", frame_w, frame_h, (int)frame_size);
    esp_camera_fb_return(fb);

    /* 双缓冲区 */
    buf[0] = heap_caps_malloc(frame_size, MALLOC_CAP_SPIRAM);
    buf[1] = heap_caps_malloc(frame_size, MALLOC_CAP_SPIRAM);
    if (!buf[0] || !buf[1]) {
        ESP_LOGE(TAG, "缓冲区分配失败");
        if (buf[0]) { heap_caps_free(buf[0]); buf[0] = NULL; }
        return;
    }

    sem = xSemaphoreCreateCounting(2, 0);
    running = true;
    BaseType_t ret = xTaskCreate(capture_task, "cam_cap", 4096, NULL, 5, &task);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "任务创建失败");
        vSemaphoreDelete(sem); sem = NULL;
        heap_caps_free(buf[0]); buf[0] = NULL;
        heap_caps_free(buf[1]); buf[1] = NULL;
        running = false;
        return;
    }
    ESP_LOGI(TAG, "采集服务启动");
}

void camera_service_stop(void)
{
    if (!running) return;
    running = false;
    vTaskDelay(pdMS_TO_TICKS(150));
    if (sem) { vSemaphoreDelete(sem); sem = NULL; }
    if (buf[0]) { heap_caps_free(buf[0]); buf[0] = NULL; }
    if (buf[1]) { heap_caps_free(buf[1]); buf[1] = NULL; }
    ESP_LOGI(TAG, "采集服务停止");
}

uint8_t *camera_service_get_frame(void)
{
    if (!sem) return NULL;
    return (xSemaphoreTake(sem, 0) == pdTRUE) ? ready_buf : NULL;
}

void camera_service_get_resolution(uint16_t *w, uint16_t *h)
{
    if (w) *w = frame_w;
    if (h) *h = frame_h;
}