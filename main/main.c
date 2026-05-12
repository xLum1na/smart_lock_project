#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "bsp_esp32s3.h"
#include "bsp_lcd.h"
#include "bsp_config.h"

static const char *TAG = "lvgl_v9_demo";

// LCD 分辨率（使用 BSP 中的定义）
#define LCD_HOR_RES   BSP_DISPLAY_WIDTH_PX
#define LCD_VER_RES   BSP_DISPLAY_HIGHT_PX   // 注意：BSP 中原名如此

// LVGL 时间基准回调函数（返回当前系统毫秒数）
static uint32_t my_tick_get_cb(void)
{
    // 使用 esp_timer 获取微秒时间戳并转换为毫秒
    return (uint32_t)(esp_timer_get_time() / 1000);
}

// 显示刷新回调：将 LVGL 渲染的缓冲区数据写入 LCD
static void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_buf)
{
    // 调用 BSP 的 LCD 绘制函数（参数为：起始/结束坐标，RGB565 数据指针）
    bsp_lcd_draw_bitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, (uint16_t *)px_buf);
    
    // 通知 LVGL 刷新完成
    lv_display_flush_ready(disp);
}

void app_main(void)
{
    ESP_LOGI(TAG, "LVGL V9 demo starting...");

    // 1. 初始化 BSP（包括 LCD 等外设）
    bsp_init();
    bsp_lcd_display_on_off(true);   // 开启 LCD 显示
    bsp_lcd_set_backlight(100);     // 设置背光亮度（0-100）
    bsp_lcd_mirror(true, true);

    // 2. LVGL 核心初始化
    lv_init();

    // 4. 设置 LVGL 时间基准（用于动画和超时）
    lv_tick_set_cb(my_tick_get_cb);

    // 5. 创建显示设备
    lv_display_t *display = lv_display_create(LCD_HOR_RES, LCD_VER_RES);
    if (!display) {
        ESP_LOGE(TAG, "Display creation failed");
        return;
    }
    ESP_LOGI(TAG, "1");
    // 6. 分配显示缓冲区（部分刷新模式，1/10 屏幕高度，RGB565）
    //    大小 = 宽度 × 行数 × 每像素字节数（2字节）
    uint32_t buf_size = LCD_HOR_RES * (LCD_VER_RES / 10) * 2;
    uint8_t *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    if (!buf1) {
        ESP_LOGE(TAG, "Failed to allocate display buffer, fallback to internal RAM");
        buf1 = malloc(buf_size);  // 尝试内部 RAM
        if (!buf1) {
            ESP_LOGE(TAG, "Display buffer allocation failed");
            return;
        }
    }
    uint8_t *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    if (!buf2) {
        ESP_LOGE(TAG, "Failed to allocate display buffer, fallback to internal RAM");
        buf2 = malloc(buf_size);  // 尝试内部 RAM
        if (!buf2) {
            ESP_LOGE(TAG, "Display buffer allocation failed");
            return;
        }
    }
    ESP_LOGI(TAG, "2");
    lv_display_set_buffers(display, buf1, buf2, buf_size, 0);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(display, my_flush_cb);

    // 7. 创建简单 UI：居中标签
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x303030), 0);
    
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL V9!\nESP32-S3");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    ESP_LOGI(TAG, "3");
    // 8. 主循环：定期调用 LVGL 任务处理器
    while (1) {
        lv_timer_handler();          // 处理 LVGL 任务（重绘、动画等）
        vTaskDelay(pdMS_TO_TICKS(15)); // 延时 5ms，让出 CPU
    }
}