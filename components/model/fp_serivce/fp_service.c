/**
 * @file   fp_service.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  指纹数据服务（Model层）实现文件
 *
 * 封装 bsp_fp 硬件层，增加状态管理与参数校验。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "fp_service.h"
#include "bsp_fp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "fp_srv";

static bool initialized = false;
static volatile bool cancel_requested = false;

uint8_t fp_service_init(void)
{
    if (initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return 1;
    }
    if (!bsp_fp_init()) {
        ESP_LOGE(TAG, "BSP FP init failed");
        return 0;
    }
    bsp_fp_set_mode(1);
    bsp_fp_recognition_enable(1);
    bsp_fp_led_enable(1);

    initialized = true;
    ESP_LOGI(TAG, "FP service initialized");
    return 1;
}

uint8_t fp_service_deinit(void)
{
    if (!initialized) return 1;
    initialized = false;
    bsp_fp_recognition_enable(0);
    bsp_fp_led_enable(0);
    return bsp_fp_deinit();
}

uint8_t fp_service_enroll(uint8_t id)
{
    if (!initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return 0;
    }
    if (id < FP_ID_MIN || id > FP_ID_MAX) {
        ESP_LOGE(TAG, "Invalid ID: %d (range %d~%d)", id, FP_ID_MIN, FP_ID_MAX);
        return 0;
    }
    ESP_LOGI(TAG, "Enrolling finger ID=%d...", id);
    uint8_t ret = bsp_fp_add_finger(id);
    ESP_LOGI(TAG, "Enroll ID=%d %s", id, ret ? "OK" : "FAIL");
    return ret;
}

uint8_t fp_service_delete(uint8_t id)
{
    if (!initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return 0;
    }
    if (id < FP_ID_MIN || id > FP_ID_MAX) {
        ESP_LOGE(TAG, "Invalid ID: %d", id);
        return 0;
    }
    ESP_LOGI(TAG, "Deleting finger ID=%d", id);
    return bsp_fp_delete_finger(id);
}

uint8_t fp_service_delete_all(void)
{
    if (!initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return 0;
    }
    ESP_LOGI(TAG, "Deleting all fingers");
    return bsp_fp_delete_all();
}

uint8_t fp_service_verify(uint8_t *matched_id, uint32_t timeout_ms)
{
    if (!initialized || !matched_id) {
        return 0;
    }
    cancel_requested = false;

    /* 分片轮询，每 100ms 检查是否取消 */
    uint32_t elapsed = 0;
    const uint32_t slice_ms = 100;

    while (!cancel_requested) {
        uint32_t wait_ms = (timeout_ms == 0) ? slice_ms
                       : (timeout_ms - elapsed < slice_ms ? timeout_ms - elapsed : slice_ms);

        if (bsp_fp_verify_finger(matched_id, wait_ms)) {
            ESP_LOGI(TAG, "Verify OK, ID=%d", *matched_id);
            return 1;
        }
        if (timeout_ms > 0) {
            elapsed += slice_ms;
            if (elapsed >= timeout_ms) break;
        }
    }

    if (cancel_requested) {
        ESP_LOGI(TAG, "Verify cancelled");
    }
    return 0;
}

void fp_service_cancel_verify(void)
{
    cancel_requested = true;
}