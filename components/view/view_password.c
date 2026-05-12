/**
 * @file   view_password.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  密码输入视图（View层）实现文件
 *
 * 布局说明 (320×480)：
 *   上部160px —— 标题文字 + 6位密码掩码指示点（●/○）
 *   下部320px —— 3×4 数字键盘（1-9、0、退格、确认）
 *
 * 遵循MVP模式：按键事件通过回调通知Presenter，显示更新由Presenter调用。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "view_password.h"
#include "lvgl.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "view_password";

/* ================================================================
 *  静态状态变量
 * ================================================================ */

static lv_obj_t *password_screen = NULL;        /* 密码页面屏幕 */
static lv_obj_t *dot_indicators[PASSWORD_MAX_LEN]; /* 6个密码掩码圆点 */
static lv_obj_t *keypad_btns[12];              /* 12个键盘按钮 */
static password_key_cb_t key_callback = NULL;   /* 按键回调函数 */
static void *key_callback_arg = NULL;           /* 回调上下文参数 */
static bool is_created = false;

/* 预定义样式 */
static lv_style_t style_dot_empty;              /* 圆点：未输入 */
static lv_style_t style_dot_filled;             /* 圆点：已输入 */
static lv_style_t style_btn_number;             /* 按钮：数字键 */
static lv_style_t style_btn_del;                /* 按钮：退格键 */
static lv_style_t style_btn_ok;                 /* 按钮：确认键 */

/* 按钮索引到按键字符的映射表（3×4 布局，从左到右、从上到下） */
static const char btn_key_map[12] = {
    '1', '2', '3',           /* 第一行 */
    '4', '5', '6',           /* 第二行 */
    '7', '8', '9',           /* 第三行 */
    'B', '0', 'C'            /* 第四行：退格、零、确认 */
};

/* 按钮标签文本 */
static const char *btn_labels[12] = {
    "1", "2", "3",
    "4", "5", "6",
    "7", "8", "9",
    "X", "0", "OK"
};

/* ================================================================
 *  样式初始化
 * ================================================================ */
static void init_styles(void)
{
    /* ---- 圆点：未输入（空心圆环）---- */
    lv_style_init(&style_dot_empty);
    lv_style_set_bg_opa(&style_dot_empty, LV_OPA_TRANSP);
    lv_style_set_border_color(&style_dot_empty, lv_color_hex(0x555555));
    lv_style_set_border_width(&style_dot_empty, 2);
    lv_style_set_border_opa(&style_dot_empty, LV_OPA_COVER);
    lv_style_set_radius(&style_dot_empty, LV_RADIUS_CIRCLE);

    /* ---- 圆点：已输入（实心填充）---- */
    lv_style_init(&style_dot_filled);
    lv_style_set_bg_color(&style_dot_filled, lv_color_hex(0x00d2ff));
    lv_style_set_bg_opa(&style_dot_filled, LV_OPA_COVER);
    lv_style_set_border_width(&style_dot_filled, 0);
    lv_style_set_radius(&style_dot_filled, LV_RADIUS_CIRCLE);

    /* ---- 数字按钮 ---- */
    lv_style_init(&style_btn_number);
    lv_style_set_bg_color(&style_btn_number, lv_color_hex(0x2d2d44));
    lv_style_set_bg_opa(&style_btn_number, LV_OPA_COVER);
    lv_style_set_radius(&style_btn_number, 10);
    lv_style_set_border_width(&style_btn_number, 0);
    lv_style_set_text_color(&style_btn_number, lv_color_white());
    lv_style_set_text_font(&style_btn_number, &lv_font_montserrat_14);
    
    /* ---- 退格按钮（琥珀色）---- */
    lv_style_init(&style_btn_del);
    lv_style_set_bg_color(&style_btn_del, lv_color_hex(0xe67e22));
    lv_style_set_bg_opa(&style_btn_del, LV_OPA_COVER);
    lv_style_set_radius(&style_btn_del, 10);
    lv_style_set_border_width(&style_btn_del, 0);
    lv_style_set_text_color(&style_btn_del, lv_color_white());
    lv_style_set_text_font(&style_btn_del, &lv_font_montserrat_14);

    /* ---- 确认按钮（绿色）---- */
    lv_style_init(&style_btn_ok);
    lv_style_set_bg_color(&style_btn_ok, lv_color_hex(0x27ae60));
    lv_style_set_bg_opa(&style_btn_ok, LV_OPA_COVER);
    lv_style_set_radius(&style_btn_ok, 10);
    lv_style_set_border_width(&style_btn_ok, 0);
    lv_style_set_text_color(&style_btn_ok, lv_color_white());
    lv_style_set_text_font(&style_btn_ok, &lv_font_montserrat_14);
}

/* ================================================================
 *  键盘事件处理
 * ================================================================ */

static void keypad_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    /* 遍历查找按钮索引 */
    for (int i = 0; i < 12; i++) {
        if (keypad_btns[i] == btn) {
            if (key_callback) {
                key_callback(btn_key_map[i], key_callback_arg);
            }
            break;
        }
    }
}

/* ================================================================
 *  视图操作接口实现
 * ================================================================ */

static void password_update_display(uint8_t count)
{
    if (count > PASSWORD_MAX_LEN) {
        count = PASSWORD_MAX_LEN;
    }
    for (uint8_t i = 0; i < PASSWORD_MAX_LEN; i++) {
        if (i < count) {
            lv_obj_add_style(dot_indicators[i], &style_dot_filled, 0);
        } else {
            lv_obj_add_style(dot_indicators[i], &style_dot_empty, 0);
        }
    }
}

static void password_clear_display(void)
{
    password_update_display(0);
}

static void password_set_key_callback(password_key_cb_t cb, void *arg)
{
    key_callback = cb;
    key_callback_arg = arg;
}

static view_password_ops_t password_ops = {
    .update_display = password_update_display,
    .clear_display = password_clear_display,
    .set_key_callback = password_set_key_callback,
};

/* ================================================================
 *  创建密码输入视图
 * ================================================================ */

uint8_t view_password_create(void)
{
    if (is_created) {
        ESP_LOGW(TAG, "Already created");
        return 1;
    }

    init_styles();

    /* ---- 创建密码页面屏幕 ---- */
    password_screen = lv_obj_create(NULL);
    if (!password_screen) {
        ESP_LOGE(TAG, "Screen create failed");
        return 0;
    }
    lv_obj_set_style_bg_color(password_screen, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_bg_opa(password_screen, LV_OPA_COVER, 0);

    /* ---- 标题：请输入密码 ---- */
    lv_obj_t *title = lv_label_create(password_screen);
    lv_label_set_text(title, "请输入密码");
    lv_obj_set_style_text_color(title, lv_color_hex(0xcccccc), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    /* ---- 密码掩码指示点：6个圆点水平排列 ---- */
    const int dot_size = 38;
    const int dot_gap = 12;
    const int dots_total_w = PASSWORD_MAX_LEN * dot_size
                           + (PASSWORD_MAX_LEN - 1) * dot_gap;
    const int dots_start_x = (320 - dots_total_w) / 2;
    const int dots_y = 85;

    for (int i = 0; i < PASSWORD_MAX_LEN; i++) {
        dot_indicators[i] = lv_obj_create(password_screen);
        lv_obj_set_size(dot_indicators[i], dot_size, dot_size);
        lv_obj_set_pos(dot_indicators[i],
                       dots_start_x + i * (dot_size + dot_gap),
                       dots_y);
        lv_obj_add_style(dot_indicators[i], &style_dot_empty, 0);
        lv_obj_set_scrollbar_mode(dot_indicators[i], LV_SCROLLBAR_MODE_OFF);
    }

    /* ---- 数字键盘：Flex 容器 + 12个独立按钮 ---- */
    lv_obj_t *keypad_container = lv_obj_create(password_screen);
    lv_obj_set_size(keypad_container, 300, 300);
    lv_obj_set_pos(keypad_container, 10, 160);
    lv_obj_set_style_bg_opa(keypad_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(keypad_container, 0, 0);
    lv_obj_set_style_pad_all(keypad_container, 0, 0);
    lv_obj_set_scrollbar_mode(keypad_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(keypad_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(keypad_container,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER);

    const int btn_w = 88;
    const int btn_h = 62;

    for (int i = 0; i < 12; i++) {
        keypad_btns[i] = lv_button_create(keypad_container);
        lv_obj_set_size(keypad_btns[i], btn_w, btn_h);

        /* 根据按钮类型应用样式 */
        if (i == 9) {           /* 退格键 */
            lv_obj_add_style(keypad_btns[i], &style_btn_del, 0);
        } else if (i == 11) {   /* 确认键 */
            lv_obj_add_style(keypad_btns[i], &style_btn_ok, 0);
        } else {                /* 数字键 */
            lv_obj_add_style(keypad_btns[i], &style_btn_number, 0);
        }

        /* 按钮标签 */
        lv_obj_t *label = lv_label_create(keypad_btns[i]);
        lv_label_set_text(label, btn_labels[i]);
        lv_obj_center(label);

        /* 绑定点击事件 */
        lv_obj_add_event_cb(keypad_btns[i], keypad_event_cb,
                            LV_EVENT_CLICKED, NULL);
    }

    is_created = true;
    ESP_LOGI(TAG, "Password view created");
    return 1;
}

/* ================================================================
 *  对外接口
 * ================================================================ */

const view_password_ops_t *view_password_get_ops(void)
{
    if (!is_created) {
        return NULL;
    }
    return &password_ops;
}

void view_password_set_visible(bool visible)
{
    if (!is_created || !password_screen) {
        return;
    }
    if (visible) {
        lv_screen_load(password_screen);
    }
}
