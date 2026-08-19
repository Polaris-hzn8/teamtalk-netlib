/**
 * @author: luochenhao
 * @email: lch2022fox@163.com
 * @time: Mon 04 May 2026 17:08:30 CST
 * @brief:
 */

#ifndef TEAMTALK_IMCORE_COMMON_TOOLS_H_
#define TEAMTALK_IMCORE_COMMON_TOOLS_H_

#include <string>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#endif

namespace teamtalk::imcore::common {

/// @brief 获取单调递增的时间戳（毫秒）
/// @return 自系统启动或高精度计时器起计的毫秒数；Windows 上优先使用 QueryPerformanceCounter
uint64_t get_tick_count();

/// @brief 阻塞当前线程指定时长
/// @param millisecond 休眠毫秒数
void util_sleep(uint32_t millisecond);

/// @brief 将当前进程 ID 写入工作目录下的 server.pid 文件
/// @note 文件打开失败时触发 assert（调试构建）
void write_pid();

/// @brief 查询本地文件大小
/// @param path 文件路径
/// @return 成功为字节数；stat 失败时为 (size_t)-1
size_t get_file_size(const char* path);

/// @brief 将数值 ID 编码为 URL 片段（36 进制 + 固定算法，首字符为版本前缀 '1'）
/// @param id 业务 ID
/// @return 编码后的字符串；与 urltoid 配对使用
std::string idtourl(uint32_t id);

/// @brief 将 idtourl 生成的字符串解码回 ID
/// @param url 编码串，通常首字符为 '1'
/// @return 解码后的 ID；空串或 nullptr 返回 0
uint32_t urltoid(const char* url);

/// @brief 将点分十进制 IPv4 字符串转为32 位主机序整数
/// @param ip 如 "a.b.c.d"
/// @return 网络字节序约定下的组合值（实现为 v1<<24|v2<<16|v3<<8|v4）
unsigned int ip2long(const char* ip);

/// @brief 将 32 位主机序整数转为点分十进制 IPv4 字符串
/// @param in 由 ip2long 同类布局的整型值
/// @return 指向静态缓冲区的 C字符串；非线程安全，勿长期保存指针
char* long2ip(const unsigned int in);

}  // namespace teamtalk::imcore::common

#endif  // TEAMTALK_IMCORE_COMMON_TOOLS_H_
