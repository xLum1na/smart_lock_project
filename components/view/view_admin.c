/**
 * @file   view_admin.c
 * @author xLumina
 * @date   2026-05-12
 * @brief  管理员设置视图（View层）实现文件
 *
 * 布局 (320×480)：顶部标题栏 + 可滚动菜单列表（5项）。
 * 每项含分类色标、功能文字、右箭头指示。
 *
 * 遵循MVP模式：菜单点击通过回调通知Presenter处理业务逻辑。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#include "view_admin.h"
#include "lvgl.h"
#include "esp_log.h"

static const char *TAG = "view_admin";

/* ================================================================
 *  静态状态
 * ================================================================ */

static lv_obj_t *admin_screen = NULL;
static lv_obj_t *menu_buttons[ADMIN_MENU_MAX];
static admin_menu_cb_t menu_callback = NULL;
static void *menu_callback_arg = NULL;
static bool is_created = false;

/* 菜单项配置 */
static const char *menu_labels[ADMIN_MENU_MAX] = {
    "人脸设置",
    "指纹设置",
    "管理员密码设置",
    "开锁密码设置",
    "日志显示",
};

/* 分类色标：人脸(蓝)、指纹(橙)、密码(绿)、日志(灰白) */
static const uint32_t menu_colors[ADMIN_MENU_MAX] = {
    0x3498db,  /* 人脸设置 - 蓝 */
    0xe67e22,  /* 指纹设置 - 橙 */
    0x27ae60,  /* 管理员密码 - 绿 */
    0x27ae60,  /* 开锁密码   - 绿 */
    0xecf0f1,  /* 日志显示 - 灰白 */
};

/* ================================================================
 *  菜单按钮点击事件
 * ================================================================ */

static void menu_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    for (int i = 0; i < ADMIN_MENU_MAX; i++) {
        if (menu_buttons[i] == btn) {
            if (menu_callback) {
                menu_callback((admin_menu_item_t)i, menu_callback_arg);
            }
            break;
        }
    }
}

/* ================================================================
 *  ops 实现
 * ================================================================ */

static void admin_set_menu_callback(admin_menu_cb_t cb, void *arg)
{
    menu_callback = cb;
    menu_callback_arg = arg;
}

static view_admin_ops_t admin_ops = {
    .set_menu_callback = admin_set_menu_callback,
};

/* ================================================================
 *  创建管理员视图
 * ================================================================ */

uint8_t view_admin_create(void)
{
    if (is_created) {
        ESP_LOGW(TAG, "Already created");
        return 1;
    }

    /* ---- 屏幕 ---- */
    admin_screen = lv_obj_create(NULL);
    if (!admin_screen) {
        ESP_LOGE(TAG, "Screen create failed");
        return 0;
    }
    lv_obj_set_style_bg_color(admin_screen, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_bg_opa(admin_screen, LV_OPA_COVER, 0);

    /* ---- 顶部标题 ---- */
    lv_obj_t *title = lv_label_create(admin_screen);
    lv_label_set_text(title, "管理员设置");
    lv_obj_set_style_text_color(title, lv_color_hex(0xcccccc), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    /* 标题底部分隔线 */
    lv_obj_t *divider = lv_obj_create(admin_screen);
    lv_obj_set_size(divider, 320, 1);
    lv_obj_set_pos(divider, 0, 52);
    lv_obj_set_style_bg_color(divider, lv_color_hex(0x333355), 0);
    lv_obj_set_style_border_width(divider, 0, 0);

    /* ---- 可滚动菜单容器 ---- */
    lv_obj_t *menu_list = lv_obj_create(admin_screen);
    lv_obj_set_size(menu_list, 320, 420);
    lv_obj_set_pos(menu_list, 0, 58);
    lv_obj_set_style_bg_opa(menu_list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(menu_list, 0, 0);
    lv_obj_set_style_pad_all(menu_list, 5, 0);
    lv_obj_set_scrollbar_mode(menu_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(menu_list, LV_DIR_VER);
    lv_obj_set_flex_flow(menu_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu_list, LV_FLEX_ALIGN_START,
                           LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    /* 每项间距 */
    lv_obj_set_style_pad_row(menu_list, 5, 0);

    /* ---- 创建5个菜单按钮 ---- */
    const int btn_w = 310;
    const int btn_h = 50;

    for (int i = 0; i < ADMIN_MENU_MAX; i++) {
        /* 按钮容器：水平flex布局 */
        menu_buttons[i] = lv_button_create(menu_list);
        lv_obj_set_size(menu_buttons[i], btn_w, btn_h);
        lv_obj_set_style_bg_color(menu_buttons[i], lv_color_hex(0x2d2d44), 0);
        lv_obj_set_style_bg_opa(menu_buttons[i], LV_OPA_COVER, 0);
        lv_obj_set_style_radius(menu_buttons[i], 8, 0);
        lv_obj_set_style_border_width(menu_buttons[i], 0, 0);
        lv_obj_set_style_pad_hor(menu_buttons[i], 14, 0);
        lv_obj_set_style_pad_ver(menu_buttons[i], 0, 0);

        /* 内部flex：色标 + 文字(撑满) + 箭头 */
        lv_obj_set_flex_flow(menu_buttons[i], LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(menu_buttons[i],
                              LV_FLEX_ALIGN_START,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_END);

        /* 分类色标圆点 */
        lv_obj_t *dot = lv_obj_create(menu_buttons[i]);
        lv_obj_set_size(dot, 14, 14);
        lv_obj_set_style_bg_color(dot, lv_color_hex(menu_colors[i]), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);

        /* 菜单文字 */
        lv_obj_t *label = lv_label_create(menu_buttons[i]);
        lv_label_set_text(label, menu_labels[i]);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_pad_left(label, 12, 0);
        /* 文字撑满剩余空间 */
        lv_obj_set_flex_grow(label, 1);

        /* 右箭头 */
        lv_obj_t *arrow = lv_label_create(menu_buttons[i]);
        lv_label_set_text(arrow, ">");
        lv_obj_set_style_text_color(arrow, lv_color_hex(0x666688), 0);
        lv_obj_set_style_text_font(arrow, &lv_font_montserrat_14, 0);

        /* 绑定点击事件 */
        lv_obj_add_event_cb(menu_buttons[i], menu_event_cb,
                            LV_EVENT_CLICKED, NULL);
    }

    is_created = true;
    ESP_LOGI(TAG, "Admin view created, %d menu items", ADMIN_MENU_MAX);
    return 1;
}

/* ================================================================
 *  对外接口
 * ================================================================ */

const view_admin_ops_t *view_admin_get_ops(void)
{
    return is_created ? &admin_ops : NULL;
}

void view_admin_set_visible(bool visible)
{
    if (is_created && visible) {
        lv_screen_load(admin_screen);
    }
}
