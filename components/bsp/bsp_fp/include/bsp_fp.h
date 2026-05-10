/**
 * @file   bsp_fp.h
 * @author xLumina
 * @date   2026-05-10
 * @brief  指纹设备抽象声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-10 | xLumina | 初始版本创建
 *
 * @par Function List:	功能函数
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 指纹设备初始化
 * 
 * 初始化指纹模块使用的 UART 外设及其他必要资源（如 GPIO、电源等）。
 * 该函数应在系统启动时调用一次，之后才能使用其他指纹功能。
 * 
 * @return uint8_t
 * @retval 1 初始化成功
 * @retval 0 初始化失败（如 UART 打开失败、参数错误等）
 */
uint8_t bsp_fp_init(void);

/**
 * @brief 指纹设备反初始化
 * 
 * 释放指纹模块占用的 UART 等资源，关闭相关外设。
 * 调用后如需再次使用指纹功能，需重新调用 bsp_fp_init()。
 * 
 * @return uint8_t
 * @retval 1 反初始化成功
 * @retval 0 反初始化失败（如尚未初始化、资源释放错误）
 */
uint8_t bsp_fp_deinit(void);

/**
 * @brief 设置指纹模块的通信波特率
 * 
 * 向模块发送波特率切换命令，成功后硬件 UART 也会同步调整波特率。
 * 支持的波特率：4800, 9600, 57600, 115200。
 * 
 * @param baud 目标波特率（数值，单位 bps）
 * @return uint8_t
 * @retval 1 设置成功
 * @retval 0 设置失败（不支持的波特率、通信超时或模块无响应）
 * 
 * @note 请确保调用前模块处于已知波特率（默认 9600），
 *       且调用后后续所有通信均使用新波特率。
 */
uint8_t bsp_fp_set_baudrate(uint32_t baud);

/**
 * @brief 注册一个新指纹（完整流程）
 * 
 * 执行完整的指纹注册流程：
 * 1. 发送注册命令（ID 由参数指定）
 * 2. 等待模块确认进入注册模式
 * 3. 等待用户按压手指 3~4 次（内部固定延时 15 秒）
 * 4. 提示用户再次按压以验证注册是否成功
 * 5. 如果验证通过则返回成功；否则自动删除该 ID 的指纹数据并返回失败。
 * 
 * @param id 要分配的指纹 ID，范围 0x00 ~ 0x80
 * @return uint8_t
 * @retval 1 注册成功，指纹已存入模块
 * @retval 0 注册失败（超时、ID 无效、手指已存在、验证错误等）
 * 
 * @note 此函数会阻塞约 25~30 秒（取决于用户按压速度）。
 *       如果注册失败，模块中不会残留该 ID 的无效数据。
 */
uint8_t bsp_fp_add_finger(uint8_t id);

/**
 * @brief 删除指定 ID 的指纹
 * 
 * 发送删除命令并从模块中清除对应 ID 的指纹模板。
 * 
 * @param id 要删除的指纹 ID，范围 0x00 ~ 0x80
 * @return uint8_t
 * @retval 1 删除成功
 * @retval 0 删除失败（ID 无效、通信失败或模块无响应）
 * 
 * @note 如果 ID 不存在，模块可能返回失败或成功（取决于模块实现），
 *       建议上层维护映射关系以避免无效删除。
 */
uint8_t bsp_fp_delete_finger(uint8_t id);

/**
 * @brief 删除所有指纹
 * 
 * 清除模块中存储的全部指纹模板数据。
 * 
 * @return uint8_t
 * @retval 1 全部删除成功
 * @retval 0 删除失败（通信超时或模块无响应）
 * 
 * @note 该操作不可逆，请确保上层有确认机制。
 */
uint8_t bsp_fp_delete_all(void);

/**
 * @brief 设置指纹模块的工作模式
 * 
 * 修改指纹识别输出的行为模式。
 * 
 * @param mode 工作模式选择（参见协议）：
 *             - 1 : 实时模式
 *             - 2 : 自锁模式
 *             - 3 : 1秒点动模式
 *             - 4 : 5秒点动模式
 *             - 5 : 10秒点动模式
 *             - 6 : 30秒点动模式
 *             - 7 : 60秒点动模式
 *             - 8 : 120秒点动模式
 * @return uint8_t
 * @retval 1 设置成功
 * @retval 0 设置失败（参数错误或通信失败）
 */
uint8_t bsp_fp_set_mode(uint8_t mode);

/**
 * @brief 开启或关闭指纹识别功能
 * 
 * 使能或禁止模块主动识别指纹并输出匹配结果。
 * 关闭后，按压手指不会触发任何串口数据。
 * 
 * @param enable 1：开启识别；0：关闭识别
 * @return uint8_t
 * @retval 1 设置成功
 * @retval 0 设置失败
 */
uint8_t bsp_fp_recognition_enable(uint8_t enable);

/**
 * @brief 开启或关闭指纹指示灯
 * 
 * 控制模块上的 LED 指示灯（通常用于引导按压）。
 * 
 * @param enable 1：开启指示灯；0：关闭指示灯
 * @return uint8_t
 * @retval 1 设置成功
 * @retval 0 设置失败
 */
uint8_t bsp_fp_led_enable(uint8_t enable);

/**
 * @brief 等待指纹匹配结果（被动验证）
 * 
 * 阻塞等待模块返回匹配成功数据包（FD AA ID BB DF）。
 * 如果在超时时间内未收到有效包或收到失败包（FD E0 E0 E0 DF），则返回 0。
 * 
 * @param id 输出参数，匹配成功时存储指纹 ID（范围 0x00 ~ 0x80）
 * @param timeout_ms 最长等待时间，单位毫秒
 * @return uint8_t
 * @retval 1 匹配成功，*id 有效
 * @retval 0 匹配失败或超时
 * 
 * @note 该函数仅读取串口数据，不会主动发送任何命令。
 *       模块应处于工作模式且识别功能开启。
 *       典型调用时机：系统待机后循环调用，等待用户按压手指。
 */
uint8_t bsp_fp_verify_finger(uint8_t *id, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif