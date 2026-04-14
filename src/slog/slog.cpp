#include "slog.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

static constexpr int kMaxMsgLen = 1024 * 4;

namespace {
std::shared_ptr<spdlog::logger> g_spdlog;
std::mutex g_mutex;
bool g_initialized = false;

spdlog::level::level_enum to_spd_level(SlogLevel level) {
  switch (level) {
    case SlogLevel::kDebug:
      return spdlog::level::debug;
    case SlogLevel::kInfo:
      return spdlog::level::info;
    case SlogLevel::kWarn:
      return spdlog::level::warn;
    case SlogLevel::kError:
      return spdlog::level::err;
    case SlogLevel::kFatal:
      return spdlog::level::critical;
    default:
      return spdlog::level::off;
  }
}

// 解析配置文件（key = value 格式）
struct SlogConf {
  std::string log_path = "./log/imcore.log";
  SlogLevel level = SlogLevel::kInfo;
  bool console = true;
  bool async = true;
  int max_size_mb = 10;
  int max_files = 5;
  size_t queue_size = 8192;
};

SlogLevel parse_level_str(const std::string& s) {
  if (s == "debug")
    return SlogLevel::kDebug;
  if (s == "info")
    return SlogLevel::kInfo;
  if (s == "warn")
    return SlogLevel::kWarn;
  if (s == "error")
    return SlogLevel::kError;
  if (s == "fatal")
    return SlogLevel::kFatal;
  if (s == "off")
    return SlogLevel::kOff;
  fprintf(stderr, "[slog] unknown level '%s', fallback to info\n", s.c_str());
  return SlogLevel::kInfo;
}

SlogConf parse_conf(const char* path) {
  SlogConf conf;
  std::ifstream ifs(path);
  if (!ifs.is_open())
    return conf;

  std::string line;
  while (std::getline(ifs, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    auto pos = line.find('=');
    if (pos == std::string::npos)
      continue;

    std::string k = line.substr(0, pos);
    std::string v = line.substr(pos + 1);
    auto trim = [](std::string& s) {
      size_t a = s.find_first_not_of(" \t\r\n");
      size_t b = s.find_last_not_of(" \t\r\n");
      s = (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
    };
    trim(k);
    trim(v);

    if (k == "log_path")
      conf.log_path = v;
    else if (k == "level")
      conf.level = parse_level_str(v);
    else if (k == "console")
      conf.console = (v == "true" || v == "1");
    else if (k == "async")
      conf.async = (v == "true" || v == "1");
    else if (k == "max_size_mb")
      conf.max_size_mb = std::stoi(v);
    else if (k == "max_files")
      conf.max_files = std::stoi(v);
    else if (k == "queue_size")
      conf.queue_size = static_cast<size_t>(std::stoi(v));
  }
  return conf;
}

bool do_init(const SlogConf& conf) {
  std::lock_guard<std::mutex> lk(g_mutex);
  if (g_initialized)
    return true;

  try {
    std::vector<spdlog::sink_ptr> sinks;
    if (!conf.log_path.empty()) {
      auto fs = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        conf.log_path, static_cast<size_t>(conf.max_size_mb) * 1024 * 1024, static_cast<size_t>(conf.max_files));
      fs->set_level(to_spd_level(conf.level));
      sinks.push_back(fs);
    }
    if (conf.console) {
      auto cs = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      cs->set_level(to_spd_level(conf.level));
      sinks.push_back(cs);
    }
    if (conf.async) {
      spdlog::init_thread_pool(conf.queue_size, 1);
      g_spdlog = std::make_shared<spdlog::async_logger>(
        "slog", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    } else {
      g_spdlog = std::make_shared<spdlog::logger>("slog", sinks.begin(), sinks.end());
    }
    g_spdlog->set_level(to_spd_level(conf.level));
    g_spdlog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%-5l%$] %v");
    g_initialized = true;
  } catch (const spdlog::spdlog_ex& e) {
    fprintf(stderr, "[slog] init failed: %s\n", e.what());
    return false;
  }
  return true;
}

}  // namespace

// -------------------------------------------------------------
// 公开函数实现
// -------------------------------------------------------------
bool slog_init(const char* config_path) {
  SlogConf conf = parse_conf(config_path);
  if (!g_initialized) {
    fprintf(stderr, "[slog] config not found: %s, using defaults\n", config_path);
  }
  return do_init(conf);
}

bool slog_init(const char* log_path, SlogLevel level, bool console) {
  SlogConf conf;
  conf.log_path = log_path ? log_path : "";
  conf.level = level;
  conf.console = console;
  return do_init(conf);
}

void slog_set_level(SlogLevel level) {
  if (g_spdlog) {
    g_spdlog->set_level(to_spd_level(level));
  }
}

void slog_shutdown() {
  std::lock_guard<std::mutex> lk(g_mutex);
  if (g_spdlog) {
    g_spdlog->flush();
    g_spdlog.reset();
  }
  spdlog::shutdown();
  g_initialized = false;
}

void slog_write(SlogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
  char msg[kMaxMsgLen];
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, sizeof(msg), fmt, args);
  va_end(args);

  // 与原 crosslog.cpp 输出格式保持一致：<%s>|<%d>|<%s>| msg
  char buf[kMaxMsgLen + 256];
  snprintf(buf, sizeof(buf), "<%s>|<%d>|<%s>| %s", file, line, func, msg);

  if (!g_initialized || !g_spdlog) {
    fprintf(stderr, "%s\n", buf);
    return;
  }
  g_spdlog->log(to_spd_level(level), buf);
}
