/**
 * @file   log_service.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  日志数据服务（Model层）实现文件
 *
 * 日志存储在 MicroSD 卡 /sdcard/smart_lock.log 文件中，
 * 每条一行："[HH:MM:SS] EVENT_TYPE detail"
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "log_service.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"

static const char *TAG = "log_srv";
static const char *LOG_FILE = "/sdcard/smart_lock.log";

static bool initialized = false;

/* 日志事件名称映射 */
static const char *event_names[] = {
    "开锁",
    "管理员登录",
    "人脸录入",
    "人脸删除",
    "指纹录入",
    "指纹删除",
    "密码修改",
    "报警",
    "系统启动",
};

uint8_t log_service_init(void)
{
    if (initialized) return 1;

    /* 检查 SD 卡是否可写 */
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) {
        ESP_LOGE(TAG, "Cannot open log file: %s", LOG_FILE);
        return 0;
    }
    fclose(fp);
    initialized = true;
    ESP_LOGI(TAG, "Log service initialized, file: %s", LOG_FILE);
    return 1;
}

uint8_t log_service_write(log_event_t event, const char *detail)
{
    if (!initialized) return 0;

    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) {
        ESP_LOGE(TAG, "Write failed: cannot open file");
        return 0;
    }

    const char *name = (event < sizeof(event_names) / sizeof(event_names[0]))
                       ? event_names[event] : "未知";

    /* 获取系统运行时间作为时间戳（秒） */
    uint32_t ts = xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
    uint32_t h = ts / 3600;
    uint32_t m = (ts % 3600) / 60;
    uint32_t s = ts % 60;

    fprintf(fp, "[%02lu:%02lu:%02lu] %s", h, m, s, name);
    if (detail) {
        fprintf(fp, " %s", detail);
    }
    fprintf(fp, "\n");
    fclose(fp);

    return 1;
}

uint32_t log_service_get_count(void)
{
    if (!initialized) return 0;

    FILE *fp = fopen(LOG_FILE, "r");
    if (!fp) return 0;

    uint32_t count = 0;
    char buf[LOG_ENTRY_MAX_LEN];
    while (fgets(buf, sizeof(buf), fp)) {
        count++;
    }
    fclose(fp);
    return count;
}

size_t log_service_read(uint32_t index, char *buf, size_t buf_len)
{
    if (!initialized || !buf || buf_len == 0) return 0;

    uint32_t total = log_service_get_count();
    if (total == 0 || index >= total) return 0;

    /* 倒序读取：index 0 = 最新行 = 文件末行 */
    uint32_t target = total - 1 - index;

    FILE *fp = fopen(LOG_FILE, "r");
    if (!fp) return 0;

    uint32_t line = 0;
    char line_buf[LOG_ENTRY_MAX_LEN];
    size_t result = 0;

    while (fgets(line_buf, sizeof(line_buf), fp)) {
        if (line == target) {
            /* 去除末尾换行 */
            size_t len = strlen(line_buf);
            if (len > 0 && line_buf[len - 1] == '\n') {
                line_buf[len - 1] = '\0';
                len--;
            }
            strncpy(buf, line_buf, buf_len - 1);
            buf[buf_len - 1] = '\0';
            result = strlen(buf);
            break;
        }
        line++;
    }
    fclose(fp);
    return result;
}

uint8_t log_service_clear(void)
{
    if (!initialized) return 0;

    FILE *fp = fopen(LOG_FILE, "w");
    if (!fp) {
        ESP_LOGE(TAG, "Clear failed");
        return 0;
    }
    fclose(fp);
    ESP_LOGI(TAG, "Log cleared");
    return 1;
}