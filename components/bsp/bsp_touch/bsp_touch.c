/**
 * @file   bsp_touch.c
 * @author xLumina
 * @date   2026-05-07
 * @brief  TOUCH 设备抽象实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-07 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_touch.h"
#include "bsp_xpt2046.h"
#include "bsp_config.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "driver/spi_master.h"
#include "esp_log.h"

static const char *TAG = "bsp_touch";

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

uint8_t bsp_touch_init(void)
{
    /* 创建touch spi io句柄 */                        
    const esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num = BSP_TOUCH_CS_PIN,
        .dc_gpio_num = -1,
        .spi_mode   = 0,
        .pclk_hz    = BSP_TOUCH_FREQ_HZ,
        .trans_queue_depth = 3,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,   
        .flags = {                                    
            .dc_high_on_cmd = 0,                    
            .dc_low_on_data = 0,                 
            .dc_low_on_param = 0,                    
            .octal_mode = 0,                            
            .quad_mode = 0,                           
            .sio_mode = 0,                         
            .lsb_first = 0,                         
            .cs_high_active = 0                   
        }                           
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, 
                    &io_cfg, &io_handle));
    const esp_lcd_touch_config_t touch_cfg = {
        .x_max = BSP_DISPLAY_WIDTH_PX,
        .y_max = BSP_DISPLAY_HIGHT_PX,
        .rst_gpio_num = -1, 
        .int_gpio_num = -1, 
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    esp_lcd_touch_new_spi_xpt2046(io_handle, &touch_cfg, &touch_handle);
    ESP_LOGI(TAG, "BSP touch init success!");
    return 1;
}

uint8_t bsp_touch_deinit(void)
{
    ESP_ERROR_CHECK(esp_lcd_touch_del(touch_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_io_del(io_handle));
    return 1;
}

uint8_t bsp_touch_read_xy(uint16_t *x, uint16_t *y)
{
    if (touch_handle == NULL) {
        ESP_LOGE("TOUCH", "touch_handle is NULL!");
        return 0;
    }
    uint16_t temp_x, temp_y;
    esp_err_t ret = esp_lcd_touch_read_data(touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("TOUCH", "esp_lcd_touch_read_data failed: %s", esp_err_to_name(ret));
        return 0;
    }

    esp_lcd_touch_point_data_t touch_points[1];
    uint8_t touch_cnt = 0;
    ret = esp_lcd_touch_get_data(touch_handle, touch_points, &touch_cnt, 1);
    if (ret != ESP_OK) {
        ESP_LOGE("TOUCH", "esp_lcd_touch_get_data failed: %s", esp_err_to_name(ret));
        return 0;
    }

    // 即使没有触摸点也记录信息
    if (touch_cnt == 0) {
        ESP_LOGD("TOUCH", "No touch points detected (touch_cnt=0)");
        return 0;
    }

    // 检查强度，但记录所有情况
    if (touch_points[0].strength == 0) {
        ESP_LOGW("TOUCH", "Touch detected but strength=0 (x=%d, y=%d)", 
                 touch_points[0].x, touch_points[0].y);
        return 0;
    }

    // 成功读取
    *x = touch_points[0].x;
    *y = touch_points[0].y;
    return 1;
}

uint8_t bsp_touch_swap_xy(bool swap)
{
    ESP_ERROR_CHECK(esp_lcd_touch_set_swap_xy(touch_handle, swap));
    return 1;
}

uint8_t bsp_touch_mirror_x(bool mirror)
{
    ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_x(touch_handle, mirror));
    return 1;
}

uint8_t bsp_touch_mirror_y(bool mirror)
{
        ESP_ERROR_CHECK(esp_lcd_touch_set_mirror_y(touch_handle, mirror));
    return 1;
}

uint8_t bsp_touch_enter_sleep(void)
{
    ESP_ERROR_CHECK(esp_lcd_touch_enter_sleep(touch_handle));
    return 1;
}

uint8_t bsp_touch_exit_sleep(void)
{
    ESP_ERROR_CHECK(esp_lcd_touch_exit_sleep(touch_handle));
    return 1;
}