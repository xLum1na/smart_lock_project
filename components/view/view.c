/**
 * @file   view.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  UI视图管理器（View层）实现文件
 *
 * LVGL显示驱动注册、双渲染缓冲、心跳定时器、子视图创建与切换。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "view.h"
#include "view_home.h"
#include "view_password.h"
#include "view_result.h"
#include "view_admin.h"
#include "bsp_config.h"
#include "bsp_lcd.h"

#include "lvgl.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "view";

static lv_display_t *disp_handle = NULL;
static view_id_t current_view = VIEW_ID_NONE;

/* ---- LVGL 显示刷新回调 ---- */
static void disp_flush_cb(lv_display_t *disp, const lv_area_t *area,
                          uint8_t *px_map)
{
    bsp_lcd_draw_bitmap(area->x1, area->y1, area->x2, area->y2, px_map);
    lv_display_flush_ready(disp);
}

/* ---- LVGL 1ms 心跳定时器 ---- */
static void tick_timer_cb(void *arg)
{
    lv_tick_inc(1);
}

uint8_t view_init(void)
{
    lv_init();

    /* 创建显示设备 */
    disp_handle = lv_display_create(BSP_DISPLAY_WIDTH_PX, BSP_DISPLAY_HIGHT_PX);
    if (!disp_handle) {
        ESP_LOGE(TAG, "Display create failed");
        return 0;
    }
    lv_display_set_flush_cb(disp_handle, disp_flush_cb);

    /* 双渲染缓冲，每块50行 */
    size_t buf_size = BSP_DISPLAY_WIDTH_PX * 50 * sizeof(lv_color_t);
    uint8_t *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    uint8_t *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    if (!buf1 || !buf2) {
        ESP_LOGE(TAG, "Buffer alloc failed");
        return 0;
    }
    lv_display_set_buffers(disp_handle, buf1, buf2, buf_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* 默认屏幕纯黑背景 */
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    /* 1ms 心跳定时器 */
    const esp_timer_create_args_t tick_args = {
        .callback = tick_timer_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t tick_timer = NULL;
    esp_timer_create(&tick_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000);

    /* 创建子视图 */
    if (!view_home_create()) {
        ESP_LOGE(TAG, "Home view create failed");
        return 0;
    }
    if (!view_password_create()) {
        ESP_LOGE(TAG, "Password view create failed");
        return 0;
    }
    if (!view_result_create()) {
        ESP_LOGE(TAG, "Result view create failed");
        return 0;
    }
    if (!view_admin_create()) {
        ESP_LOGE(TAG, "Admin view create failed");
        return 0;
    }

    /* 默认显示主页面 */
    view_switch_to(VIEW_ID_HOME);

    ESP_LOGI(TAG, "View manager init success, %dx%d",
             BSP_DISPLAY_WIDTH_PX, BSP_DISPLAY_HIGHT_PX);
    return 1;
}

uint8_t view_switch_to(view_id_t id)
{
    if (id >= VIEW_ID_MAX || id < 0) {
        ESP_LOGE(TAG, "Invalid view ID: %d", id);
        return 0;
    }

    view_home_set_visible(id == VIEW_ID_HOME);
    view_password_set_visible(id == VIEW_ID_PASSWORD);
    view_result_set_visible(id == VIEW_ID_RESULT);
    view_admin_set_visible(id == VIEW_ID_ADMIN);
    current_view = id;
    ESP_LOGI(TAG, "View switched to %d", id);
    return 1;
}

view_id_t view_get_current(void)
{
    return current_view;
}

void view_task(void *arg)
{
    ESP_LOGI(TAG, "LVGL task running");
    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}