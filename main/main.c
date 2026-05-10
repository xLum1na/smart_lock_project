#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "bsp.h"
#include "bsp_config.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"

#include "esp_lcd_io_spi.h"
#include "esp_lcd_touch.h"
#include "bsp_xpt2046.h"

#include "jpeg_decoder.h" // 引入JPEG解码库
#include "esp_camera.h" // 引入摄像头库头文件
#include "bsp_fp.h"

#define TAG "app_main"

#define LCD_WIDTH 320
#define LCD_HEIGHT 480
#define CHUNK_WIDTH LCD_WIDTH // 320
#define CHUNK_HEIGHT 40       // 每次传输40行

// --- 修改块高度 ---
#define LCD_WIDTH 320
#define LCD_HEIGHT 480
#define CHUNK_WIDTH LCD_WIDTH  // 320
#define CHUNK_HEIGHT 3         // 每次传输3行

// 辅助函数：旋转图像并分块显示
void rotate_and_display_image(uint16_t *src_buffer, uint16_t src_width, uint16_t src_height) {
    uint16_t tgt_width = LCD_WIDTH;  // 320
    uint16_t tgt_height = LCD_HEIGHT; // 480
    if(src_width != tgt_height || src_height != tgt_width) {
         ESP_LOGE(TAG, "rotate_and_display_image: Source dimensions (%dx%d) do not match rotated target (%dx%d)", 
                  src_width, src_height, tgt_width, tgt_height);
         return;
    }

    uint16_t current_tgt_row_start = 0;
    while (current_tgt_row_start < tgt_height) {
        // 计算当前块的实际高度（最后一块可能不足CHUNK_HEIGHT）
        uint16_t current_chunk_height = (tgt_height - current_tgt_row_start >= CHUNK_HEIGHT) ? 
                                         CHUNK_HEIGHT : (tgt_height - current_tgt_row_start);

        // 分配一个临时缓冲区来存储当前要绘制的块
        size_t chunk_buf_size = CHUNK_WIDTH * current_chunk_height * sizeof(uint16_t);
        uint16_t *temp_chunk_buffer = (uint16_t*)heap_caps_malloc(chunk_buf_size, MALLOC_CAP_SPIRAM);
        if (!temp_chunk_buffer) {
            ESP_LOGE(TAG, "Failed to allocate temporary chunk buffer of size %zu for row %d", chunk_buf_size, current_tgt_row_start);
            return; // 或者尝试跳过此块
        }

        // --- 旋转并填充临时块缓冲区 ---
        for (int tgt_y = current_tgt_row_start; tgt_y < current_tgt_row_start + current_chunk_height; tgt_y++) {
            for (int tgt_x = 0; tgt_x < tgt_width; tgt_x++) {
                // 旋转90度的映射关系 (tgt_x, tgt_y) <- (src_y, src_width - 1 - src_x)
                int src_y = tgt_x; // tgt_x becomes src_y
                int src_x = src_height - 1 - tgt_y; // tgt_y determines src_x, flipped
                
                uint32_t src_index = (uint32_t)src_y * src_width + src_x;
                uint32_t tgt_chunk_index = (uint32_t)(tgt_y - current_tgt_row_start) * tgt_width + tgt_x;

                temp_chunk_buffer[tgt_chunk_index] = src_buffer[src_index];
            }
        }

        // --- 绘制临时块缓冲区到LCD ---
        uint16_t x_start = 0;
        uint16_t y_start = current_tgt_row_start;
        uint16_t x_end = tgt_width - 1; // 319
        uint16_t y_end = current_tgt_row_start + current_chunk_height - 1;

        uint8_t draw_result = bsp_lcd_draw_bitmap(x_start, y_start, x_end, y_end, temp_chunk_buffer);
        
        if (draw_result != 0) {
            ESP_LOGW(TAG, "LCD draw bitmap chunk failed (returned %d) at tgt_row %d, height %d", 
                     draw_result, current_tgt_row_start, current_chunk_height);
        } else {
            ESP_LOGD(TAG, "Drawn rotated chunk at tgt_row %d, height: %d", current_tgt_row_start, current_chunk_height);
        }

        // 释放当前块的临时缓冲区
        heap_caps_free(temp_chunk_buffer);

        // 更新下一块的目标起始行
        current_tgt_row_start += current_chunk_height;

        // 可选：在块之间添加短暂延迟
        // vTaskDelay(pdMS_TO_TICKS(1)); // 延迟1ms，可以根据需要调整
    }
    
    ESP_LOGI(TAG, "Finished displaying all chunks.");
}

void display_camera_image_on_lcd() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Failed to get camera frame");
        return;
    }

    ESP_LOGI(TAG, "Got JPEG frame, size: %zu bytes, width: %d, height: %d", fb->len, fb->width, fb->height);

    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = fb->buf,
        .indata_size = fb->len,
        .outbuf = NULL,
        .outbuf_size = 0,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 0
        },
        .advanced = {
            .working_buffer = NULL,
            .working_buffer_size = 0
        }
    };

    esp_jpeg_image_output_t output_info;
    esp_err_t err = esp_jpeg_get_image_info(&jpeg_cfg, &output_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get JPEG image info (%s)", esp_err_to_name(err));
        esp_camera_fb_return(fb);
        return;
    }

    ESP_LOGI(TAG, "JPEG image info: Width=%d, Height=%d, Required output size=%zu bytes",
             output_info.width, output_info.height, output_info.output_len);

    // 检查图像尺寸是否是我们预期的（需要旋转的）尺寸: 480x320 -> 320x480
    if (!(output_info.width == LCD_HEIGHT && output_info.height == LCD_WIDTH)) { 
         ESP_LOGW(TAG, "Warning: Image dimensions (%dx%d) are not matching expected rotated source for %dx%d LCD. Logic assumes W=480, H=320.",
                  output_info.width, output_info.height, LCD_WIDTH, LCD_HEIGHT);
         esp_camera_fb_return(fb);
         return;
    }

    size_t required_outbuf_size = output_info.output_len;
    uint16_t *rgb565_buffer = (uint16_t*)heap_caps_malloc(required_outbuf_size, MALLOC_CAP_SPIRAM);
    if (!rgb565_buffer) {
        ESP_LOGE(TAG, "Failed to allocate RGB565 output buffer of size %zu", required_outbuf_size);
        esp_camera_fb_return(fb);
        return;
    }

    jpeg_cfg.outbuf = (uint8_t*)rgb565_buffer;
    jpeg_cfg.outbuf_size = required_outbuf_size;

    err = esp_jpeg_decode(&jpeg_cfg, &output_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "JPEG decode failed (%s)", esp_err_to_name(err));
        heap_caps_free(rgb565_buffer);
        esp_camera_fb_return(fb);
        return;
    }

    ESP_LOGI(TAG, "JPEG decoded successfully. Actual output size: %zu bytes", output_info.output_len);

    // 调用新的旋转和显示函数
    rotate_and_display_image(rgb565_buffer, output_info.width, output_info.height);

    ESP_LOGI(TAG, "Rotated image displayed on 320x480 LCD successfully with %dx%d chunks.", CHUNK_WIDTH, CHUNK_HEIGHT);

    // 清理资源
    heap_caps_free(rgb565_buffer);
    esp_camera_fb_return(fb);
}

void app_main(void) {

    bsp_init();

    bsp_lcd_display_on_off(true);
    bsp_lcd_set_backlight(100);

        // 先注册指纹
    if (bsp_fp_add_finger(1)) {
        ESP_LOGI(TAG, "Registration success. Now waiting for fingerprint matches...");
    } else {
        ESP_LOGE(TAG, "Registration failed.");
        return;
    }

    // 注册成功后，无限循环等待指纹匹配
    while (1) {
        uint8_t ver_id;
        if (bsp_fp_verify_finger(&ver_id, 0)) {
            ESP_LOGI(TAG, "Finger matched! ID = %d", ver_id);
            // 执行匹配成功后的动作，如开锁、点亮LED等
        } else {
            // 无匹配或超时，可以不做处理
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}