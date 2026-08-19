/**
 * @author: luochenhao
 * @email: lch2022fox@163.com
 * @time: Sun 03 May 2026 23:11:15 CST
 * @brief: 标准配置文件读写
 */

#ifndef TEAMTALK_IMCORE_CONFIG_READER_H_
#define TEAMTALK_IMCORE_CONFIG_READER_H_

#include <map>
#include <string>
#include <cstdint>
#include <utility>
#include <vector>

namespace teamtalk::imcore::config_reader {

class CConfigReader {
 public:
  explicit CConfigReader(const char* file_path);
  ~CConfigReader();

  // 获取配置值（线程安全，返回 std::string）
  std::string GetConfigValue(const std::string& key);
  std::string GetConfigValue(const char* key);

  // 类型安全的获取方法
  int GetIntValue(const std::string& key, int default_val = 0);
  int GetIntValue(const char* key, int default_val = 0);
  uint32_t GetUint32Value(const std::string& key, uint32_t default_val = 0);
  uint32_t GetUint32Value(const char* key, uint32_t default_val = 0);
  bool GetBoolValue(const std::string& key, bool default_val = false);
  bool GetBoolValue(const char* key, bool default_val = false);

  // 设置配置值
  bool SetConfigValue(const char* key, const char* value);
  bool SetConfigValue(const std::string& key, const std::string& value);
  bool SetConfigValue(const std::string& key, int value);

  // 按编号读取端点列表
  // IP前缀+端口前缀从1开始递增读取，直到某个key不存在为止
  std::vector<std::pair<std::string, uint16_t>> ReadNumberedEndpoints(const std::string& ip_prefix,
                                                                      const std::string& port_prefix);

  bool IsLoadSuccess() const { return m_load_ok; }

 private:
  bool _LoadFile(const char* file_path);
  bool _WriteFile(const char* file_path = nullptr);
  void _ParseLine(char* line);
  char* _TrimSpace(char* str);

  bool m_load_ok;                                   //是否加载成功
  std::string m_config_file;                        //配置文件路径
  std::map<std::string, std::string> m_config_map;  //配置存储
};

}  // namespace teamtalk::imcore::config_reader

#endif  // TEAMTALK_IMCORE_CONFIG_READER_H_
