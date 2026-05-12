/**
 * @file   password_service.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  密码数据服务（Model层）实现文件
 *
 * 使用 ESP-IDF NVS（非易失性存储）持久化密码数据。
 * NVS 键名：admin_pwd / unlock_pwd
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "password_service.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "pwd_srv";
static const char *NVS_NAMESPACE = "password";

/* NVS 键名 */
#define KEY_ADMIN_PWD   "admin_pwd"
#define KEY_UNLOCK_PWD  "unlock_pwd"

static bool initialized = false;

uint8_t password_service_init(void)
{
    if (initialized) return 1;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
        return 0;
    }

    initialized = true;
    ESP_LOGI(TAG, "Password service initialized");
    return 1;
}

static uint8_t pwd_set(const char *key, const char *pwd)
{
    if (!initialized || !pwd) return 0;
    if (strlen(pwd) != PASSWORD_MAX_LEN) {
        ESP_LOGW(TAG, "Password length must be %d, got %d",
                 PASSWORD_MAX_LEN, (int)strlen(pwd));
        return 0;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed");
        return 0;
    }

    err = nvs_set_str(handle, key, pwd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS set %s failed", key);
        nvs_close(handle);
        return 0;
    }

    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "%s set OK", key);
    return 1;
}

static uint8_t pwd_verify(const char *key, const char *pwd)
{
    if (!initialized || !pwd) return 0;

    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return 0;
    }

    size_t len = 0;
    esp_err_t err = nvs_get_str(handle, key, NULL, &len);
    if (err != ESP_OK || len == 0) {
        nvs_close(handle);
        return 0;
    }

    char stored[PASSWORD_MAX_LEN + 1];
    err = nvs_get_str(handle, key, stored, &len);
    nvs_close(handle);

    if (err != ESP_OK) return 0;

    return (strncmp(pwd, stored, PASSWORD_MAX_LEN) == 0) ? 1 : 0;
}

static bool pwd_is_set(const char *key)
{
    if (!initialized) return false;

    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    size_t len = 0;
    esp_err_t err = nvs_get_str(handle, key, NULL, &len);
    nvs_close(handle);

    return (err == ESP_OK && len > 0);
}

uint8_t password_service_set_admin(const char *pwd)
{
    return pwd_set(KEY_ADMIN_PWD, pwd);
}

uint8_t password_service_set_unlock(const char *pwd)
{
    return pwd_set(KEY_UNLOCK_PWD, pwd);
}

uint8_t password_service_verify_admin(const char *pwd)
{
    return pwd_verify(KEY_ADMIN_PWD, pwd);
}

uint8_t password_service_verify_unlock(const char *pwd)
{
    return pwd_verify(KEY_UNLOCK_PWD, pwd);
}

bool password_service_admin_is_set(void)
{
    return pwd_is_set(KEY_ADMIN_PWD);
}

bool password_service_unlock_is_set(void)
{
    return pwd_is_set(KEY_UNLOCK_PWD);
}