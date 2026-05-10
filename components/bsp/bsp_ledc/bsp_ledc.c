/**
 * @file   bsp_ledc.c
 * @author xLumina
 * @date   2026-05-07
 * @brief  LEDC 外设初始化实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_ledc.h"
#include "bsp_config.h"

#include "driver/ledc.h"
#include "esp_log.h"


static const char *TAG = "bsp_ledc";



uint8_t bsp_ledc_init(void)
{
    esp_err_t ret; 
    const ledc_timer_config_t tim_cfg = {
        .timer_num          = BSP_LEDC_TIMER,
        .speed_mode         = BSP_LEDC_MODE,
        .clk_cfg            = BSP_LEDC_CLK,
        .duty_resolution    = BSP_LEDC_DUTY_RES,
        .freq_hz            = BSP_LEDC_FREQ_HZ,
    };
    ret = ledc_timer_config(&tim_cfg);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "BSP ledc timer config fail!!!");    
        return 0;
    }
    const ledc_channel_config_t chan_cfg = {
        .timer_sel  = BSP_LEDC_TIMER,
        .speed_mode = BSP_LEDC_MODE,
        .channel    = BSP_LEDC_CHANNEL,
        .gpio_num   = BSP_DISPLAY_LEDC_PIN,
        .duty   = 0,
        .hpoint = 0,
    };
    ret = ledc_channel_config(&chan_cfg);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "BSP ledc channel config fail!!!");    
        return 0;
    }
    ESP_LOGI(TAG, "BSP ledc init success!");
    return 1;
}


uint8_t bsp_ledc_deinit(void)
{
    esp_err_t ret; 

    ESP_LOGI(TAG, "BSP ledc deinit success!");
    return 1;
}
