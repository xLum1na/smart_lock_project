/**
 * @file   bsp.c
 * @author xLumina
 * @date   2026-05-06
 * @brief  bsp驱动文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-06 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */


#include "bsp.h"
#include "bsp_config.h"

#include "bsp_ledc.h"
#include "bsp_fp.h"
#include "bsp_spi.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "freertos/FreeRTOS.h"
#include "bsp_camera.h"
#include "esp_log.h"

SemaphoreHandle_t spi_mutex = NULL;

void bsp_init(void)
{

    ESP_LOGI("BSP", "Starting BSP initialization...");
    
    bsp_ledc_init();
    ESP_LOGI("BSP", "LEDC initialized");
    
    bsp_spi_bus_init();
    ESP_LOGI("BSP", "SPI bus initialized");
    
    bsp_lcd_init();  
    ESP_LOGI("BSP", "LCD initialized");
    // bsp_camera_init();
    // bsp_touch_init();
    bsp_fp_init();


    

}


