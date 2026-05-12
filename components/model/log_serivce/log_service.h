/**
 * @file   log_service.h
 * @author xLumina
 * @date   2026-05-12
 * @brief  日志数据服务（Model层）声明文件
 *
 * 使用 MicroSD 卡持久化存储系统事件日志。
 *
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-12 | xLumina | 初始版本创建
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 单条日志最大长度 */
#define LOG_ENTRY_MAX_LEN  128

/**
 * @brief 日志事件类型枚举
 */
typedef enum {
    LOG_EVENT_UNLOCK         = 0,  ///< 开锁事件
    LOG_EVENT_ADMIN_ENTER    = 1,  ///< 进入管理员模式
    LOG_EVENT_FACE_ENROLL    = 2,  ///< 人脸录入
    LOG_EVENT_FACE_DELETE    = 3,  ///< 人脸删除
    LOG_EVENT_FP_ENROLL      = 4,  ///< 指纹录入
    LOG_EVENT_FP_DELETE      = 5,  ///< 指纹删除
    LOG_EVENT_PWD_CHANGE     = 6,  ///< 密码修改
    LOG_EVENT_ALARM          = 7,  ///< 报警事件
    LOG_EVENT_SYSTEM_START   = 8,  ///< 系统启动
} log_event_t;

/**
 * @brief 初始化日志服务（依赖 SD 卡已挂载到 /sdcard）
 * @return 1 成功，0 失败（SD 卡未就绪等）
 */
uint8_t log_service_init(void);

/**
 * @brief 写入一条日志
 * @param event  事件类型
 * @param detail 详情文字（可为 NULL）
 * @return 1 成功，0 失败
 */
uint8_t log_service_write(log_event_t event, const char *detail);

/**
 * @brief 获取日志总数
 * @return 日志条目数量，失败返回 0
 */
uint32_t log_service_get_count(void);

/**
 * @brief 读取指定索引的日志条目
 * @param index  日志索引（0 为最新，count-1 为最旧）
 * @param buf    输出缓冲区
 * @param buf_len 缓冲区大小
 * @return 实际写入的字符数，失败或无数据返回 0
 */
size_t log_service_read(uint32_t index, char *buf, size_t buf_len);

/**
 * @brief 清空全部日志
 * @return 1 成功，0 失败
 */
uint8_t log_service_clear(void);

#ifdef __cplusplus
}
#endif