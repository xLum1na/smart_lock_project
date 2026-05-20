/**
 * @file   bsp_servo.c
 * @author xLumina
 * @date   2026-05-18
 * @brief  舵机 设备驱动实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-18 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_servo.h"
#include "bsp_config.h"


#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"


static const char *TAG = "SG90_DRIVER";

uint8_t bsp_servo_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,  // 14位分辨率，提供更精确的控制
        .timer_num = BSP_LEDC_TIMER,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP ledc timer cfg fail: %s", esp_err_to_name(ret));
        return 0;
    }
    ledc_channel_config_t ledc_channel = {
        .gpio_num = BSP_SERVO_LEDC_PIIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = BSP_SERVO_LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = BSP_LEDC_TIMER,
        .duty = 0,  // 初始占空比为0
        .hpoint = 0,
    };
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP ledc channel cfg fail: %s", esp_err_to_name(ret));
        return 0;
    }

    ESP_LOGI(TAG, "BSP servo init success");
    return 1;
}

/**
 * @brief 舵机 设备反初始化函数
 */
uint8_t bsp_servo_deinit(void)
{
    ESP_LOGI(TAG, "BSP servo deinit success");
    return 1;
}


uint8_t bsp_servo_set_angle(uint16_t angle)
{
    if (angle > 180) {
        ESP_LOGW(TAG, "BSP servo angle fail: %d", angle);
        angle = 180;
    }

    // 计算对应的脉冲宽度 (微秒)
    float pulse_width = BSP_SERVO_MIN_PW_US + 
                       (angle / 180.0) * (BSP_SERVO_MAX_PW_US - BSP_SERVO_MIN_PW_US);

    // 将脉冲宽度转换为占空比 (14位分辨率)
    uint32_t duty_cycle = (uint32_t)(pulse_width * 16383 / 20000);  // 16383 = 2^14 - 1

    // 设置占空比
    esp_err_t ret = ledc_set_duty(LEDC_LOW_SPEED_MODE, BSP_SERVO_LEDC_CHANNEL, duty_cycle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP set angle fail: %s", esp_err_to_name(ret));
        return 0;
    }

    // 更新LED输出
    ret = ledc_update_duty(LEDC_LOW_SPEED_MODE, BSP_SERVO_LEDC_CHANNEL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP set angle fail: %s", esp_err_to_name(ret));
        return 0;
    }

    ESP_LOGI(TAG, "BSP set angle success：angle = %d", angle);
    return 1;
}