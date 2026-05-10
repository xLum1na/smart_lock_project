/**
 * @file   bsp_fp.c
 * @author xLumina
 * @date   2026-05-10
 * @brief  指纹设备抽象实现文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-10 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#include "bsp_fp.h"
#include "bsp_config.h"


#include "driver/uart.h"
#include "esp_log.h"


static const char *TAG = "bsp_fp";

uint8_t bsp_fp_init(void)
{
    ESP_ERROR_CHECK(uart_driver_install(BSP_FP_UART_PORT, 256, 256, 0, NULL, 0));
    const uart_config_t uart_cfg = {
        .baud_rate = BSP_FP_UART_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(BSP_FP_UART_PORT, &uart_cfg));

    ESP_ERROR_CHECK(uart_set_pin(BSP_FP_UART_PORT, BSP_FP_UART_TX_PIN, 
                                BSP_FP_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "fp device init success");
    return 1;
}

uint8_t bsp_fp_deinit(void)
{
    ESP_ERROR_CHECK(uart_driver_delete(BSP_FP_UART_PORT));
    ESP_LOGI(TAG, "fp device deinit success");
    return 1;
}

void uart_flush_rx(uart_port_t port) 
{
    uint8_t dummy[32];
    uart_read_bytes(port, dummy, sizeof(dummy), 10); // 超时丢弃
}

uint8_t bsp_fp_set_baudrate(uint32_t baud)
{
    uint8_t cmd[5] = {0xFD, 0xF1, 0x00, 0x00, 0xDF};
    switch (baud) {
        case 4800:  cmd[3] = 0x01; break;
        case 9600:  cmd[3] = 0x02; break;
        case 57600: cmd[3] = 0x03; break;
        case 115200:cmd[3] = 0x04; break;
        default: return 0;
    }

    uart_flush_rx(BSP_FP_UART_PORT);
    if (uart_write_bytes(BSP_FP_UART_PORT, cmd, 5) <= 0) return 0;

    uint8_t resp;
    int len = uart_read_bytes(BSP_FP_UART_PORT, &resp, 1, pdMS_TO_TICKS(1000));
    if (len == 1 && resp == 0xF1) {
        // 改变硬件波特率
        uart_set_baudrate(BSP_FP_UART_PORT, baud);
        ESP_LOGI(TAG, "Baudrate set to %d", baud);
        return 1;
    }
    return 0;
}

uint8_t bsp_fp_add_finger(uint8_t id)
{
    if (id >80) {
        return 0;
    }
    /* 刷uart接收缓冲区 */
    uart_flush_rx(BSP_FP_UART_PORT);
    /* 构造注册指纹数据包 */
    uint8_t cmd_packet[5] = {0xFD, 0xA1, 0x00, id, 0xDF};
    int len = uart_write_bytes(BSP_FP_UART_PORT, cmd_packet, 5);
    if (len <= 0) {
        ESP_LOGE(TAG, "Send add command failed");
        return 0;
    }
    /* 接收返回数据包，判断命令是否有效 */
    uint8_t ack;
    int read_len = uart_read_bytes(BSP_FP_UART_PORT, &ack, 1, pdMS_TO_TICKS(2000));
    if (read_len != 1 || ack != 0xA1) {
        ESP_LOGE(TAG, "Module did not ack add command");
        return 0;
    }
    ESP_LOGI(TAG, "Module ready, please press finger 3-4 times");
    /* 注册指纹延时，延时10秒进行指纹录入 */
    vTaskDelay(pdMS_TO_TICKS(10000));
    ESP_LOGI(TAG, "Now press your finger again to verify registration...");
    /* 验证指纹是否录入成功 */
    uint8_t verified_id;
    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(10000)) {
        if (bsp_fp_verify_finger(&verified_id, pdMS_TO_TICKS(500))) {
            if (verified_id == id) {
                /* 录入成功 */
                ESP_LOGI(TAG, "Finger %02X registered successfully", id);
                return 1;
            } else {
                /* 录入失败，可能此指纹已经录入了，有其他id，删除刚刚注册的指纹id */
                bsp_fp_delete_finger(id);
                ESP_LOGE(TAG, "Verified ID mismatch: expected %02X, got %02X", id, verified_id);
                return 0;
            }
        }
        // 未收到包则短暂等待重试
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ESP_LOGE(TAG, "Timeout waiting for verification press");
    return 0;
}

uint8_t bsp_fp_delete_finger(uint8_t id)
{
    /* 清空接收缓冲区 */
    uart_flush_rx(BSP_FP_UART_PORT);

    /* 构造删除命令包 */
    uint8_t cmd_packet[5] = {0xFD, 0xB1, 0x00, id, 0xDF};

    /* 发送命令 */
    int len = uart_write_bytes(BSP_FP_UART_PORT, cmd_packet, 5);
    if (len <= 0) {
        ESP_LOGE(TAG, "Failed to send delete command for ID %02X", id);
        return 0;
    }
    /* 等待返回数据包，判断命令是否有效 */
    uint8_t response;
    int read_len = uart_read_bytes(BSP_FP_UART_PORT, &response, 1, pdMS_TO_TICKS(1000));
    if (read_len == 1 && response == 0xB1) {
        ESP_LOGI(TAG, "Fingerprint ID %02X deleted successfully", id);
        return 1;
    } else {
        ESP_LOGE(TAG, "Delete fingerprint ID %02X failed (response: %02X)", id, response);
        return 0;
    }
}

uint8_t bsp_fp_delete_all(void)
{
    uint8_t cmd[5] = {0xFD, 0xD0, 0x00, 0x00, 0xDF};

    uart_flush_rx(BSP_FP_UART_PORT);
    if (uart_write_bytes(BSP_FP_UART_PORT, cmd, 5) <= 0) return 0;

    uint8_t resp;
    int len = uart_read_bytes(BSP_FP_UART_PORT, &resp, 1, pdMS_TO_TICKS(2000));
    if (len == 1 && resp == 0xD0) {
        ESP_LOGI(TAG, "All fingerprints deleted");
        return 1;
    }
    return 0;
}

uint8_t bsp_fp_set_mode(uint8_t mode)
{
    if (mode < 1 || mode > 8) return 0;
    uint8_t cmd[5] = {0xFD, 0xC1, 0x00, mode, 0xDF};

    uart_flush_rx(BSP_FP_UART_PORT);
    if (uart_write_bytes(BSP_FP_UART_PORT, cmd, 5) <= 0) return 0;

    // 协议未明确反馈，假设成功返回0xC1
    uint8_t resp;
    int len = uart_read_bytes(BSP_FP_UART_PORT, &resp, 1, pdMS_TO_TICKS(1000));
    if (len == 1 && resp == 0xC1) {
        ESP_LOGI(TAG, "Mode set to %d", mode);
        return 1;
    }
    return 0;
}

uint8_t bsp_fp_recognition_enable(uint8_t enable)
{
    uint8_t cmd[5] = {0xFD, 0xE1, 0x00, enable ? 0x01 : 0x00, 0xDF};

    uart_flush_rx(BSP_FP_UART_PORT);
    if (uart_write_bytes(BSP_FP_UART_PORT, cmd, 5) <= 0) return 0;

    uint8_t resp;
    int len = uart_read_bytes(BSP_FP_UART_PORT, &resp, 1, pdMS_TO_TICKS(1000));
    if (len == 1 && resp == 0xE1) {
        ESP_LOGI(TAG, "Fingerprint recognition %s", enable ? "enabled" : "disabled");
        return 1;
    }
    return 0;
}

uint8_t bsp_fp_led_enable(uint8_t enable)
{
    uint8_t cmd[5] = {0xFD, 0xE1, 0x00, enable ? 0x03 : 0x02, 0xDF};

    uart_flush_rx(BSP_FP_UART_PORT);
    if (uart_write_bytes(BSP_FP_UART_PORT, cmd, 5) <= 0) return 0;

    uint8_t resp;
    int len = uart_read_bytes(BSP_FP_UART_PORT, &resp, 1, pdMS_TO_TICKS(1000));
    if (len == 1 && resp == 0xE1) {
        ESP_LOGI(TAG, "LED %s", enable ? "enabled" : "disabled");
        return 1;
    }
    return 0;
}

uint8_t bsp_fp_verify_finger(uint8_t *id, uint32_t timeout_ms)
{
    uint8_t packet[5];
    /* 获取指纹ID */
    int read_len = uart_read_bytes(BSP_FP_UART_PORT, packet, 5, timeout_ms);
    if (read_len != 5) {
        return 0;
    }
    if (packet[0] == 0xFD && packet[1] == 0xAA && packet[4] == 0xDF) {
        *id = packet[2];   
        return 1;
    }
    return 0;
}

