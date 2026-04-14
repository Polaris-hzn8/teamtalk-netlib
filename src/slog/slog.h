#ifndef SLOG_H
#define SLOG_H

/**
 * @file    slog.h
 * @brief   日志模块，底层封装 spdlog
 *
 * 使用方式：
 *
 *   // 1. 进程启动时初始化一次（二选一）
 *   slog_init("./conf/log.conf");                            // 从配置文件读取
 *   slog_init("./log/imcore.log", SlogLevel::kInfo, true);  // 代码直接指定
 *
 *   // 2. 使用宏打印日志，自动附带文件名/行号/函数名
 *   log_info("server started on port %d", port);
 *   log_warn("retry count: %d", retry);
 *   log_error("connect failed: %s", err_msg);
 *
 *   // 3. 进程退出前调用，确保异步队列全部落盘
 *   slog_shutdown();
 *
 * 输出格式：
 *   [2026-04-12 10:30:01.234] [INFO ] <msg_server.cpp>|<45>|<handleLogin>| server started
 */

// -------------------------------------------------------------
// 日志级别
// -------------------------------------------------------------
enum class SlogLevel {
  kDebug = 0,
  kInfo = 1,
  kWarn = 2,
  kError = 3,
  kFatal = 4,
  kOff = 5,
};

// -------------------------------------------------------------
// 初始化 / 运行时控制 / 关闭
// -------------------------------------------------------------

/// @brief 从配置文件初始化日志系统（推荐，格式见 conf/log.conf）
/// @param config_path 配置文件路径，文件不存在时使用默认值
/// @return 初始化成功返回 true
bool slog_init(const char* config_path);

/// @brief 代码直接初始化日志系统
/// @param log_path    日志文件路径，传 nullptr 或空串则不写文件
/// @param level       最低输出级别，见 SlogLevel
/// @param console     是否同时输出到控制台，默认 true
/// @return 初始化成功返回 true
bool slog_init(const char* log_path, SlogLevel level, bool console = true);

/// @brief 运行时动态调整日志级别，无需重启进程
/// @param level 新的日志级别，见 SlogLevel
void slog_set_level(SlogLevel level);

/// @brief 刷盘并关闭日志系统，进程退出前必须调用
void slog_shutdown();

// -------------------------------------------------------------
// 内部写入函数，请勿直接调用，使用下方宏
// -------------------------------------------------------------
void slog_write(SlogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

#ifdef _WIN32
#define SLOG_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define SLOG_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

// -------------------------------------------------------------
// 日志宏（唯一推荐的使用方式）
// -------------------------------------------------------------
#define log_debug(fmt, ...) slog_write(SlogLevel::kDebug, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) slog_write(SlogLevel::kInfo, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) slog_write(SlogLevel::kWarn, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) slog_write(SlogLevel::kError, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) slog_write(SlogLevel::kFatal, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#endif  // SLOG_H
