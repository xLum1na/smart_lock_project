/**
 * @file   presenter.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  Presenter 层公共实现文件
 *
 * 统一管理各子 Presenter 的初始化与生命周期。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "presenter.h"
#include "presenter_home.h"
#include "esp_log.h"

static const char *TAG = "presenter";

uint8_t presenter_init(void)
{
    /* 初始化主页面 Presenter（内部初始化 Model 并获取 View 引用） */
    if (!presenter_home_init()) {
        ESP_LOGE(TAG, "Home presenter init failed");
        return 0;
    }

    ESP_LOGI(TAG, "Presenter layer init success");
    return 1;
}