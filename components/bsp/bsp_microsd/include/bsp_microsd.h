/**
 * @file   bsp_microsd.h
 * @author xLumina
 * @date   2026-05-11
 * @brief  MICROSD 设备抽象声明文件
 * 
 * @par History:	修改历史
 *   - V1.0.0 | 2026-05-11 | xLumina | 初始版本创建
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
 * @brief 初始化 MicroSD 卡
 * 
 * 该函数会配置 SPI 总线、初始化 SD 卡、挂载 FAT 文件系统到 "/sdcard" 路径。
 * 
 * @return uint8_t 0 - 成功, 1 - 失败
 */
uint8_t bsp_microsd_init(void);

/**
 * @brief 反初始化 MicroSD 卡
 * 
 * 该函数会卸载 FAT 文件系统，并清理 SPI 总线资源。
 * 
 * @return uint8_t 0 - 成功, 1 - 失败
 */
uint8_t bsp_microsd_deinit(void);

// /**
//  * @brief 打开一个文件
//  * @param path 文件路径 (e.g., "/sdcard/myfile.txt")
//  * @param mode 打开模式 ("r", "w", "a", "rb", "wb", "ab" 等)
//  * @return 文件指针 (FILE*)，失败则返回 NULL
//  */
// FILE *bsp_microsd_fopen(const char *path, const char *mode);

// /**
//  * @brief 关闭一个文件
//  * @param stream 文件指针
//  * @return 成功返回 0，失败返回 EOF (-1)
//  */
// int bsp_microsd_fclose(FILE *stream);

// /**
//  * @brief 从文件读取数据
//  * @param ptr 指向目标缓冲区的指针
//  * @param size 每个元素的大小 (in bytes)
//  * @param count 要读取的元素数量
//  * @param stream 文件指针
//  * @return 成功读取的元素数量
//  */
// size_t bsp_microsd_fread(void *ptr, size_t size, size_t count, FILE *stream);

// /**
//  * @brief 向文件写入数据
//  * @param ptr 指向源数据缓冲区的指针
//  * @param size 每个元素的大小 (in bytes)
//  * @param count 要写入的元素数量
//  * @param stream 文件指针
//  * @return 成功写入的元素数量
//  */
// size_t bsp_microsd_fwrite(const void *ptr, size_t size, size_t count, FILE *stream);

// /**
//  * @brief 获取文件当前位置指示器
//  * @param stream 文件指针
//  * @return 当前位置偏移量 (long)，失败返回 -1L
//  */
// long bsp_microsd_ftell(FILE *stream);

// /**
//  * @brief 设置文件位置指示器
//  * @param stream 文件指针
//  * @param offset 偏移量
//  * @param whence SEEK_SET, SEEK_CUR, SEEK_END
//  * @return 成功返回 0，失败返回 -1
//  */
// int bsp_microsd_fseek(FILE *stream, long offset, int whence);

// /**
//  * @brief 将文件位置指示器设置为开头
//  * @param stream 文件指针
//  */
// void bsp_microsd_rewind(FILE *stream);

// /**
//  * @brief 检查文件结束标志
//  * @param stream 文件指针
//  * @return 如果设置了结束标志则返回非零值，否则返回 0
//  */
// int bsp_microsd_feof(FILE *stream);

// /**
//  * @brief 检查文件错误标志
//  * @param stream 文件指针
//  * @return 如果设置了错误标志则返回非零值，否则返回 0
//  */
// int bsp_microsd_ferror(FILE *stream);

// /**
//  * @brief 清除文件错误和结束标志
//  * @param stream 文件指针
//  */
// void bsp_microsd_clearerr(FILE *stream);

// /**
//  * @brief 刷新输出缓冲区
//  * @param stream 文件指针 (NULL 表示刷新所有)
//  * @return 成功返回 0，失败返回 EOF
//  */
// int bsp_microsd_fflush(FILE *stream);


#ifdef __cplusplus
}
#endif