
/**
 * @file    slog.h
 * @brief   日志模块，底层封装 spdlog
 *
 * 用法：
 *
 *   // 1. 进程启动时初始化一次（二选一）
 *   Slog::Default().Init("./conf/log.conf");                            // 从配置文件读取
 *   Slog::Default().Init("./log/imcore.log", SlogLevel::kInfo, true);  // 代码直接指定
 *
 *   // 2. 使用宏打印日志，自动附带文件名/行号/函数名
 *   log_info("server started on port %d", port);
 *   log_warn("retry count: %d", retry);
 *   log_error("connect failed: %s", err_msg);
 *
 *   // 3. 析构时自动 flush/shutdown，也可手动提前调用
 *   Slog::Default().Shutdown();
 *
 * 输出格式：
 *   [2026-04-12 10:30:01.234] [INFO ] <msg_server.cpp>|<45>|<handleLogin>| server started
 */

#ifndef TEAMTALK_IMCORE_SLOG_SLOG_H_
#define TEAMTALK_IMCORE_SLOG_SLOG_H_

#include <memory>
#include <cstring>

namespace teamtalk::imcore::slog {

enum class SlogLevel {
  kDebug = 0,  // 调试级别
  kInfo = 1,   // 信息级别
  kWarn = 2,   // 警告级别
  kError = 3,  // 错误级别
  kFatal = 4,  // 严重错误级别
  kOff = 5,    // 关闭级别
};

class SlogImpl;

class Slog {
 public:
  Slog();
  ~Slog();

  Slog(const Slog&) = delete;
  Slog& operator=(const Slog&) = delete;

  /// @brief 从配置文件初始化（推荐，格式见 conf/log.conf）
  /// @param config_path 配置文件路径，文件不存在时使用默认值
  /// @return 初始化成功返回 true
  bool Init(const char* config_path);

  /// @brief 直接用代码参数初始化
  /// @param log_path 日志文件路径，传 nullptr 或空串则不写文件
  /// @param level    最低输出级别
  /// @param console  是否同时输出到控制台
  /// @return 初始化成功返回 true
  bool Init(const char* log_path, SlogLevel level, bool console = true);

  /// @brief 运行时动态调整日志级别，无需重启进程
  void SetLevel(SlogLevel level);

  /// @brief 刷盘并关闭；析构时自动调用，也可手动提前触发
  void Shutdown();

  /// @brief 写入一条日志，建议通过下方宏调用
  void Write(SlogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

  /// @brief 进程级默认实例（宏的调用目标）
  static Slog& Default();

 private:
  std::unique_ptr<SlogImpl> impl_;
};

}  // namespace teamtalk::imcore::slog

#ifdef _WIN32
#define SLOG_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define SLOG_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define log_debug(fmt, ...)                        \
  ::teamtalk::imcore::slog::Slog::Default().Write( \
    ::teamtalk::imcore::slog::SlogLevel::kDebug, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)                         \
  ::teamtalk::imcore::slog::Slog::Default().Write( \
    ::teamtalk::imcore::slog::SlogLevel::kInfo, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)                         \
  ::teamtalk::imcore::slog::Slog::Default().Write( \
    ::teamtalk::imcore::slog::SlogLevel::kWarn, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)                        \
  ::teamtalk::imcore::slog::Slog::Default().Write( \
    ::teamtalk::imcore::slog::SlogLevel::kError, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)                        \
  ::teamtalk::imcore::slog::Slog::Default().Write( \
    ::teamtalk::imcore::slog::SlogLevel::kFatal, SLOG_FILENAME, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#endif  // TEAMTALK_IMCORE_SLOG_SLOG_H_
