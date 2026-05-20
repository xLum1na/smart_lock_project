/**
 * @file   bsp_camera.c
 * @author xLumina
 * @date   2026-05-10
 * @brief  CAMERA 设备抽象实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-10 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_camera.h"
#include "bsp_config.h"

#include "esp_camera.h"
#include "esp_log.h"

static const char *TAG = "bsp_camera";

uint8_t bsp_camera_init(void)
{
    const camera_config_t camera_cfg = {
        .pin_pwdn = BSP_CAMERA_PWDN_PIN,
        .pin_reset = BSP_CAMERA_RESET_PIN,
        .pin_xclk = BSP_CAMERA_XCLK_PIN,
        .pin_sccb_sda = BSP_CAMERA_SIOD_PIN,
        .pin_sccb_scl = BSP_CAMERA_SIOC_PIN,

        .pin_d7 = BSP_CAMERA_DATA7_PIN,
        .pin_d6 = BSP_CAMERA_DATA6_PIN,
        .pin_d5 = BSP_CAMERA_DATA5_PIN,
        .pin_d4 = BSP_CAMERA_DATA4_PIN,
        .pin_d3 = BSP_CAMERA_DATA3_PIN,
        .pin_d2 = BSP_CAMERA_DATA2_PIN,
        .pin_d1 = BSP_CAMERA_DATA1_PIN,
        .pin_d0 = BSP_CAMERA_DATA0_PIN,
        .pin_vsync = BSP_CAMERA_VSYNC_PIN,
        .pin_href = BSP_CAMERA_HREF_PIN,
        .pin_pclk = BSP_CAMERA_PCLK_PIN,

        //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
        .xclk_freq_hz = BSP_CAMERA_XCLK_FREQ_HZ,
        .ledc_timer = BSP_CAMERA_LEDC_TIMER,
        .ledc_channel = BSP_CAMERA_LEDC_CHANNEL,

        .pixel_format = PIXFORMAT_RGB565,
        .frame_size = FRAMESIZE_HVGA,    // 480×320 全屏

        .jpeg_quality = 10, //0-63, for OV series camera sensors, lower number means higher quality
        .fb_count = 2,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
        .fb_location = CAMERA_FB_IN_PSRAM,
        .grab_mode = CAMERA_GRAB_LATEST,
    };
    ESP_ERROR_CHECK(esp_camera_init(&camera_cfg));
    ESP_LOGI(TAG, "BSP camera init success!");
    return 1;
}

uint8_t bsp_camera_deinit(void)
{
    ESP_ERROR_CHECK(esp_camera_deinit());
    ESP_LOGI(TAG, "BSP camera deinit success!");
    return 1;
}


