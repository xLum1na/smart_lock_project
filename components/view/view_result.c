/**
 * @file   view_result.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  验证结果展示视图（View层）实现文件
 *
 * 布局 (320×480)：居中大圆环图标 + 验证类型文字。
 * 成功=绿色，失败=红色。Presenter 调用 show() 显示结果并自行控制跳转。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "view_result.h"
#include "lvgl.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "view_result";

static lv_obj_t *result_screen = NULL;
static lv_obj_t *icon_circle = NULL;
static lv_obj_t *icon_label = NULL;
static lv_obj_t *text_label = NULL;
static bool is_created = false;

/* 样式 */
static lv_style_t style_circle_ok;
static lv_style_t style_circle_fail;
static lv_style_t style_icon_ok;
static lv_style_t style_icon_fail;
static lv_style_t style_text_ok;
static lv_style_t style_text_fail;

/* 文字映射 */
static const char *type_names[] = { "人脸验证", "指纹验证", "密码验证" };
static const char *result_names[] = { "成功", "失败" };

static void init_styles(void)
{
    lv_style_init(&style_circle_ok);
    lv_style_set_bg_opa(&style_circle_ok, LV_OPA_TRANSP);
    lv_style_set_border_color(&style_circle_ok, lv_color_hex(0x27ae60));
    lv_style_set_border_width(&style_circle_ok, 4);
    lv_style_set_radius(&style_circle_ok, LV_RADIUS_CIRCLE);

    lv_style_init(&style_circle_fail);
    lv_style_set_bg_opa(&style_circle_fail, LV_OPA_TRANSP);
    lv_style_set_border_color(&style_circle_fail, lv_color_hex(0xe74c3c));
    lv_style_set_border_width(&style_circle_fail, 4);
    lv_style_set_radius(&style_circle_fail, LV_RADIUS_CIRCLE);

    lv_style_init(&style_icon_ok);
    lv_style_set_text_color(&style_icon_ok, lv_color_hex(0x27ae60));
    lv_style_set_text_font(&style_icon_ok, &lv_font_montserrat_14);

    lv_style_init(&style_icon_fail);
    lv_style_set_text_color(&style_icon_fail, lv_color_hex(0xe74c3c));
    lv_style_set_text_font(&style_icon_fail, &lv_font_montserrat_14);

    lv_style_init(&style_text_ok);
    lv_style_set_text_color(&style_text_ok, lv_color_hex(0x27ae60));
    lv_style_set_text_font(&style_text_ok, &lv_font_montserrat_14);

    lv_style_init(&style_text_fail);
    lv_style_set_text_color(&style_text_fail, lv_color_hex(0xe74c3c));
    lv_style_set_text_font(&style_text_fail, &lv_font_montserrat_14);
}

static void clear_obj_style(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
}

/* ================================================================
 *  ops 实现
 * ================================================================ */

static void result_show(verify_type_t type, verify_result_t result)
{
    if (!is_created) return;

    bool ok = (result == VERIFY_RESULT_SUCCESS);

    clear_obj_style(icon_circle);
    lv_obj_add_style(icon_circle, ok ? &style_circle_ok : &style_circle_fail, 0);

    clear_obj_style(icon_label);
    lv_label_set_text(icon_label, ok ? "OK" : "X");
    lv_obj_add_style(icon_label, ok ? &style_icon_ok : &style_icon_fail, 0);

    clear_obj_style(text_label);
    char msg[64];
    snprintf(msg, sizeof(msg), "%s%s", type_names[type], result_names[result]);
    lv_label_set_text(text_label, msg);
    lv_obj_add_style(text_label, ok ? &style_text_ok : &style_text_fail, 0);

    lv_screen_load(result_screen);
    ESP_LOGI(TAG, "%s", msg);
}

static view_result_ops_t result_ops = { .show = result_show };

/* ================================================================
 *  对外接口
 * ================================================================ */

uint8_t view_result_create(void)
{
    if (is_created) return 1;

    init_styles();

    result_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(result_screen, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_bg_opa(result_screen, LV_OPA_COVER, 0);

    icon_circle = lv_obj_create(result_screen);
    lv_obj_set_size(icon_circle, 100, 100);
    lv_obj_align(icon_circle, LV_ALIGN_CENTER, 0, -40);

    icon_label = lv_label_create(icon_circle);
    lv_label_set_text(icon_label, "OK");
    lv_obj_center(icon_label);

    text_label = lv_label_create(result_screen);
    lv_obj_align(text_label, LV_ALIGN_CENTER, 0, 80);

    is_created = true;
    ESP_LOGI(TAG, "Result view created");
    return 1;
}

const view_result_ops_t *view_result_get_ops(void)
{
    return is_created ? &result_ops : NULL;
}

void view_result_set_visible(bool visible)
{
    if (is_created && visible) {
        lv_screen_load(result_screen);
    }
}