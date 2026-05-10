/**
 * @file   bsp_gpio.c
 * @author xLumina
 * @date   2026-05-07
 * @brief  GPIO 外设初始化实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_gpio.h"
#include "bsp_config.h"

#include "driver/gpio.h"
#include "esp_log.h"


static const char *TAG = "bsp_gpio";


uint8_t bsp_gpio_init(void)
{
    esp_err_t ret; 
    const gpio_config_t io_cfg = {
        .pin_bit_mask = 1ULL << BSP_DISPLAY_DC_PIN | 1ULL << BSP_DISPLAY_RESET_PIN,
        .mode           = GPIO_MODE_OUTPUT,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_ENABLE,
        .intr_type      = GPIO_INTR_DISABLE,
    };
    ret = gpio_config(&io_cfg);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "BSP gpio config fail!!!");
        return 0;
    }
    ESP_LOGI(TAG, "BSP gpio init success!");
    return 1;
}


uint8_t bsp_gpio_deinit(void)
{
    esp_err_t ret; 

    ESP_LOGI(TAG, "BSP gpio deinit success!");
    return 1;
}
