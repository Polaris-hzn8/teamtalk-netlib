
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fstream>

#include <teamtalk/imcore/slog/slog.h>
#include <teamtalk/imcore/string/string.h>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace teamtalk::imcore::slog {

static constexpr int kMaxMsgLen = 1024 * 4;

namespace {
namespace ttstring = teamtalk::imcore::string;

struct SlogConf {
  std::string name_ = "teamtalk";
  std::string log_path_ = "./log/default.log";
  SlogLevel level_ = SlogLevel::kInfo;
  bool console_ = true;
  bool async_ = true;
  int32_t max_size_mb_ = 10;
  int32_t max_files_ = 5;
  size_t queue_size_ = 8192;
};

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
  if (!ifs.is_open()) {
    return conf;
  }

  std::string line;
  while (std::getline(ifs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    auto pos = line.find('=');
    if (pos == std::string::npos) {
      continue;
    }

    std::string k = line.substr(0, pos);
    std::string v = line.substr(pos + 1);
    ttstring::str_trim(k);
    ttstring::str_trim(v);
    if (k.empty() || v.empty()) {
      continue;
    }

    int parsed = 0;
    if (k == "log_path") {
      conf.log_path_ = v;
    } else if (k == "level") {
      conf.level_ = parse_level_str(v);
    } else if (k == "console") {
      conf.console_ = (v == "true" || v == "1");
    } else if (k == "async") {
      conf.async_ = (v == "true" || v == "1");
    } else if (k == "max_size_mb") {
      if (ttstring::str_to_int(v, parsed) && parsed >= 1) {
        conf.max_size_mb_ = parsed;
      }
    } else if (k == "max_files") {
      if (ttstring::str_to_int(v, parsed) && parsed >= 1) {
        conf.max_files_ = parsed;
      }
    } else if (k == "queue_size") {
      if (ttstring::str_to_int(v, parsed) && parsed >= 64) {
        conf.queue_size_ = static_cast<size_t>(parsed);
      }
    }
  }
  return conf;
}

}  // namespace

// -------------------------------------------------------------
// SlogImpl
// -------------------------------------------------------------
class SlogImpl {
 public:
  std::shared_ptr<spdlog::logger> logger_;
  std::mutex mtx_;
  bool initialized_ = false;

  bool do_init(const SlogConf& conf) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (initialized_)
      return true;

    try {
      std::vector<spdlog::sink_ptr> sinks;
      if (!conf.log_path_.empty()) {
        auto fs = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
          conf.log_path_, static_cast<size_t>(conf.max_size_mb_) * 1024 * 1024, static_cast<size_t>(conf.max_files_));
        fs->set_level(to_spd_level(conf.level_));
        sinks.push_back(fs);
      }
      if (conf.console_) {
        auto cs = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        cs->set_level(to_spd_level(conf.level_));
        sinks.push_back(cs);
      }

      if (conf.async_) {
        spdlog::init_thread_pool(conf.queue_size_, 1);
        logger_ = std::make_shared<spdlog::async_logger>(
          conf.name_, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
      } else {
        logger_ = std::make_shared<spdlog::logger>(conf.name_, sinks.begin(), sinks.end());
      }
      logger_->set_level(to_spd_level(conf.level_));
      logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%-5l%$] %v");
      initialized_ = true;
    } catch (const spdlog::spdlog_ex& e) {
      fprintf(stderr, "[slog] init failed: %s\n", e.what());
      return false;
    }
    return true;
  }

  void shutdown() {
    std::lock_guard<std::mutex> lk(mtx_);
    if (!initialized_)
      return;
    if (logger_) {
      logger_->flush();
      logger_.reset();
    }
    spdlog::shutdown();
    initialized_ = false;
  }
};

// -------------------------------------------------------------
// Slog 公开方法
// -------------------------------------------------------------
Slog::Slog() : impl_(std::make_unique<SlogImpl>()) {}

Slog::~Slog() {
  Shutdown();
}

bool Slog::Init(const char* config_path) {
  SlogConf conf = parse_conf(config_path);
  {
    std::ifstream f(config_path);
    if (!f.is_open()) {
      fprintf(stderr, "[slog] config not found: %s, using defaults\n", config_path);
    }
  }
  return impl_->do_init(conf);
}

bool Slog::Init(const char* log_path, SlogLevel level, bool console) {
  SlogConf conf;
  conf.log_path_ = log_path ? log_path : "";
  conf.level_ = level;
  conf.console_ = console;
  return impl_->do_init(conf);
}

void Slog::SetLevel(SlogLevel level) {
  if (impl_->logger_) {
    impl_->logger_->set_level(to_spd_level(level));
  }
}

void Slog::Shutdown() {
  impl_->shutdown();
}

void Slog::Write(SlogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
  char msg[kMaxMsgLen];
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, sizeof(msg), fmt, args);
  va_end(args);

  char buf[kMaxMsgLen + 256];
  snprintf(buf, sizeof(buf), "<%s>|<%d>|<%s>| %s", file, line, func, msg);

  if (!impl_->initialized_ || !impl_->logger_) {
    fprintf(stderr, "%s\n", buf);
    return;
  }
  impl_->logger_->log(to_spd_level(level), buf);
}

// static —— Meyer's singleton，C++11 保证线程安全初始化
Slog& Slog::Default() {
  static Slog instance;
  return instance;
}

}  // namespace teamtalk::imcore::slog
