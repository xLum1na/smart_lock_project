/**
 * @file   bsp_spi.c
 * @author xLumina
 * @date   2026-05-07
 * @brief  SPI 外设总线初始化实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_spi.h"
#include "bsp_config.h"

#include "driver/spi_master.h"
#include "esp_log.h"


static const char *TAG = "bsp_ledc";


uint8_t bsp_spi_bus_init(void)
{
    esp_err_t ret; 
    const spi_bus_config_t lcd_bus_cfg = {
        .mosi_io_num = BSP_SPI_MOSI_PIN,
        .miso_io_num = BSP_SPI_MISO_PIN,
        .sclk_io_num = BSP_SPI_SCLK_PIN,
        .max_transfer_sz =BSP_SPI_TRAN_MAX_SIZE,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
    };
    ret = spi_bus_initialize(BSP_SPI_HOST, &lcd_bus_cfg, BSP_SPI_DMA_CHAN);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "BSP spi bus init fail!!!");    
        return 0;
    }

    ESP_LOGI(TAG, "BSP spi bus init success!");
    return 1;
}

uint8_t bsp_spi_bus_deinit(void)
{
    esp_err_t ret; 
    ret = spi_bus_free(BSP_SPI_HOST);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "BSP spi bus deinit fail!!!");    
        return 0;
    }
    ESP_LOGI(TAG, "BSP spi bus deinit success!");
    return 1;
}
