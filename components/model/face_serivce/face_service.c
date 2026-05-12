/**
 * @file   face_service.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  人脸识别数据服务（Model层）实现文件
 *
 * 当前为占位实现（stub），待集成 ESP-WHO 或其他人脸识别算法后补充。
 * 接口已就绪，Presenter 可直接调用。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "face_service.h"
#include "esp_log.h"

static const char *TAG = "face_srv";
static bool initialized = false;

uint8_t face_service_init(void)
{
    if (initialized) return 1;
    initialized = true;
    ESP_LOGW(TAG, "Face service stub initialized (not implemented)");
    return 1;
}

uint8_t face_service_deinit(void)
{
    initialized = false;
    return 1;
}

uint8_t face_service_enroll(uint8_t id)
{
    if (!initialized) return 0;
    if (id < FACE_ID_MIN || id > FACE_ID_MAX) return 0;
    ESP_LOGW(TAG, "Face enroll ID=%d: not implemented (stub)", id);
    return 0;
}

uint8_t face_service_delete(uint8_t id)
{
    if (!initialized) return 0;
    if (id < FACE_ID_MIN || id > FACE_ID_MAX) return 0;
    ESP_LOGW(TAG, "Face delete ID=%d: not implemented (stub)", id);
    return 0;
}

uint8_t face_service_delete_all(void)
{
    ESP_LOGW(TAG, "Face delete all: not implemented (stub)");
    return 0;
}

uint8_t face_service_verify(uint8_t *matched_id)
{
    if (!initialized || !matched_id) return 0;
    ESP_LOGW(TAG, "Face verify: not implemented (stub)");
    return 0;
}