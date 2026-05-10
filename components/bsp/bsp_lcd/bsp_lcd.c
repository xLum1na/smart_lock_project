/**
 * @file   bsp_lcd.c
 * @author xLumina
 * @date   2026-05-07
 * @brief  lcd 设备抽象实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */
#include "bsp_lcd.h"
#include "ili9488.h"
#include "bsp_config.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_io_spi.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "bsp_lcd";

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t lcd_handle = NULL;

uint8_t bsp_lcd_init(void)
{
    const esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num = BSP_DISPLAY_CS_PIN,
        .dc_gpio_num = BSP_DISPLAY_DC_PIN,
        .spi_mode   = BSP_DISPLAY_SPI_MODE,
        .pclk_hz    = BSP_DISPLAY_SPI_FREQ_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits   = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_cfg, &io_handle));

    const esp_lcd_panel_dev_config_t dev_cfg = {
        .reset_gpio_num = BSP_DISPLAY_RESET_PIN,
        .color_space    = ESP_LCD_COLOR_SPACE_BGR,
        .bits_per_pixel = 18,
    };

    size_t buffer_size = BSP_DISPLAY_WIDTH_PX * 3 * 2;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9488(io_handle, &dev_cfg, buffer_size, &lcd_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_handle, true));
    ESP_LOGI(TAG, "BSP lcd init success!");
    return 1;
}


uint8_t bsp_lcd_deinit(void)
{
    if (lcd_handle) {
        esp_lcd_panel_del(lcd_handle);
        lcd_handle = NULL;
    }
    if (io_handle) {
        esp_lcd_panel_io_del(io_handle);
        io_handle = NULL;
    }
    return 1;
}

uint8_t bsp_lcd_display_on_off(bool on)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_disp_on_off(lcd_handle, on);
    return (ret == ESP_OK) ? 1 : 0;
}

uint8_t bsp_lcd_sleep(bool sleep)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_disp_sleep(lcd_handle, sleep);
    return (ret == ESP_OK) ? 1 : 0;
}


uint8_t bsp_lcd_mirror(bool x, bool y)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_mirror(lcd_handle, x, y);
    return (ret == ESP_OK) ? 1 : 0;
}


uint8_t bsp_lcd_swap_xy(bool swap)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_swap_xy(lcd_handle, swap);
    return (ret == ESP_OK) ? 1 : 0;
}


uint8_t bsp_lcd_set_gap(int x_gap, int y_gap)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_set_gap(lcd_handle, x_gap, y_gap);
    return (ret == ESP_OK) ? 1 : 0;
}


uint8_t bsp_lcd_invert_color(bool invert)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_invert_color(lcd_handle, invert);
    return (ret == ESP_OK) ? 1 : 0;
}


uint8_t bsp_lcd_draw_bitmap(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0,
                        const void *pixels)
{
    if (!lcd_handle) return 0;
    esp_err_t ret = esp_lcd_panel_draw_bitmap(lcd_handle,
                                               x, y,  
                                               x0, y0, 
                                               pixels);
    return (ret == ESP_OK) ? 1 : 0;
}

uint8_t bsp_lcd_set_backlight(uint8_t percent)
{
    if (percent > 100) percent = 100;
    uint32_t duty = (percent * 4095) / 100;
    ledc_set_duty(BSP_LEDC_MODE, BSP_LEDC_CHANNEL, duty);
    ledc_update_duty(BSP_LEDC_MODE, BSP_LEDC_CHANNEL);
    return 1;
}



