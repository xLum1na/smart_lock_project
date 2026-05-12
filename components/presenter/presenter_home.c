/**
 * @file   presenter_home.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  主页面协调器（Presenter层）实现文件
 *
 * 数据流：camera_service_get_frame() → view_home_ops->update_frame()
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "presenter_home.h"
#include "camera_service.h"
#include "view_home.h"
#include "esp_log.h"

static const char *TAG = "presenter_home";

/* Presenter 持有的 Model 和 View 引用 */
static const view_home_ops_t *home_view_ops = NULL;
static bool initialized = false;

uint8_t presenter_home_init(void)
{
    if (initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return 1;
    }

    /* 初始化 Model：摄像头服务 */
    if (!camera_service_init()) {
        ESP_LOGE(TAG, "Camera service init failed");
        return 0;
    }

    /* 获取 View：主页面操作接口 */
    home_view_ops = view_home_get_ops();
    if (!home_view_ops) {
        ESP_LOGE(TAG, "Home view ops is NULL");
        return 0;
    }

    initialized = true;
    ESP_LOGI(TAG, "Home presenter initialized");
    return 1;
}

uint8_t presenter_home_tick(void)
{
    if (!initialized || !home_view_ops) {
        return 0;
    }

    camera_frame_t frame;

    /* 从 Model 获取一帧 */
    if (!camera_service_get_frame(&frame)) {
        ESP_LOGW(TAG, "Get frame failed");
        return 0;
    }

    /* 推送到 View 显示 */
    uint8_t ret = home_view_ops->update_frame(frame.data,
                                               frame.width,
                                               frame.height);

    /* 释放帧缓冲 */
    camera_service_release_frame(&frame);

    return ret;
}