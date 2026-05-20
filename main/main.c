/**
 * @file   main.c
 * @author xLumina
 * @date   2026-05-20
 * @brief  智能门锁 - camera_service(生产) + display_service(消费)
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "bsp_esp32s3.h"
#include "bsp_lcd.h"
#include "esp_camera.h"

#include "camera_service.h"
#include "display_service.h"

static const char *TAG = "main";

void app_main(void)
{
    bsp_init();
    bsp_lcd_display_on_off(true);
    bsp_lcd_set_backlight(100);
    bsp_lcd_swap_xy(true);

    /* 传感器亮度 */
    sensor_t *s = esp_camera_sensor_get();
    if (s) {
        s->set_brightness(s, 2);
        s->set_contrast(s, 1);
        s->set_saturation(s, 1);
        s->set_exposure_ctrl(s, 1);
        s->set_gain_ctrl(s, 1);
        s->set_agc_gain(s, 30);
        s->set_aec_value(s, 600);
    }

    camera_service_start();
    display_service_init();

    uint32_t n = 0;
    int64_t t0 = esp_timer_get_time();

    while (1) {
        int64_t t1 = esp_timer_get_time();
        display_service_update();
        int64_t t2 = esp_timer_get_time();

        n++;
        if (t2 - t0 >= 1000000) {
            ESP_LOGI(TAG, "FPS: %.1f  frame:%lldms",
                     n * 1e6f / (t2 - t0), (t2 - t1) / 1000);
            n = 0; t0 = t2;
        }
        vTaskDelay(1);
    }
}