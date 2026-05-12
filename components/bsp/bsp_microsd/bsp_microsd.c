/**
 * @file   bsp_microsd.c
 * @author xLumina
 * @date   2026-05-11
 * @brief  MICROSD 设备抽象实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-11 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_microsd.h"
#include "bsp_config.h"

#include "driver/spi_master.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"

static const char *TAG = "bsp_microsd";


// 全局变量，用于保存 SD 卡句柄，以便在 deinit 时使用
static sdmmc_card_t* s_card_handle = NULL;

// 全局变量，用于标记 SD 卡是否已挂载
static bool s_is_mounted = false;

uint8_t bsp_microsd_init(void)
{
    /* 检查是否已经初始化过 */
    if (s_is_mounted) {
        ESP_LOGW(TAG, "SD card already mounted. Re-initializing...");
        bsp_microsd_deinit();
    }

    /* SD SPI 设备配置 */
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = BSP_SD_CS_PIN;
    slot_config.host_id = BSP_SPI_HOST; // 这里设置了实际的 HOST ID

    /* 创建 SDMMC 主机配置，必须与 slot_config 中的 host_id 匹配 */
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // 关键修改：显式设置 host 的 spi_host，使其与 slot_config 匹配
    host.slot = BSP_SPI_HOST; 

    /* 配置挂载参数 */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    /* 挂载文件系统 */
    esp_err_t ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, 
                                            &mount_config, &s_card_handle);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true in mount_config.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). Check wiring, power, or card insertion.", esp_err_to_name(ret));
        }
        s_card_handle = NULL;
        s_is_mounted = false;
        return 1; // 失败返回 1
    }
    
    ESP_LOGI(TAG, "SD card initialized and mounted successfully.");
    s_is_mounted = true;

    /* 打印 SD 卡信息 */
    sdmmc_card_print_info(stdout, s_card_handle);
    return 0; // 成功返回 0
}

uint8_t bsp_microsd_deinit(void)
{
    if (!s_is_mounted) {
        ESP_LOGW(TAG, "SD card not mounted, nothing to do.");
        return 1; // 已经卸载，返回成功
    }

    ESP_LOGI(TAG, "Unmounting SD card...");

    // --- 卸载文件系统 ---
    esp_err_t ret = esp_vfs_fat_sdcard_unmount("/sdcard", s_card_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount SD card (%s)", esp_err_to_name(ret));
        return 0; // 返回失败
    }
    s_card_handle = NULL; // 清空句柄
    s_is_mounted = false; // 更新状态
    ESP_LOGI(TAG, "SD card unmounted.");
    return 1; // 返回成功
}






