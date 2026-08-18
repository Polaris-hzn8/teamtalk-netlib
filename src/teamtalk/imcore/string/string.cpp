/**
 * @author: luochenhao
 * @email: lch2022fox@163.com
 * @time: Fri 01 May 2026 21:20:27 CST
 * @brief:
 */

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <teamtalk/imcore/string/string.h>

namespace teamtalk::imcore::string {

std::string url_encode(const std::string& s) {
  std::string out;
  for (size_t i = 0; i < s.size(); i++) {
    unsigned char buf[4] = {};
    if (isalnum((unsigned char)s[i])) {
      buf[0] = s[i];
    } else {
      buf[0] = '%';
      buf[1] = to_hex((unsigned char)s[i] >> 4);
      buf[2] = to_hex((unsigned char)s[i] % 16);
    }
    out += (char*)buf;
  }
  return out;
}

std::string url_decode(const std::string& s) {
  std::string out;
  for (size_t i = 0; i < s.size(); i++) {
    unsigned char ch = 0;
    if (s[i] == '%') {
      ch = (from_hex(s[i + 1]) << 4);
      ch |= from_hex(s[i + 2]);
      i += 2;
    } else if (s[i] == '+') {
      ch = ' ';
    } else {
      ch = s[i];
    }
    out += (char)ch;
  }
  return out;
}

void str_trim(std::string& s) {
  const size_t first = s.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    s.clear();
    return;
  }
  const size_t last = s.find_last_not_of(" \t\r\n");
  s = s.substr(first, last - first + 1);
}

bool str_to_int(const std::string& s, int& out) {
  try {
    out = std::stoi(s);
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

bool str_iequals(const std::string& str1, const std::string& str2) {
  if (str1.size() != str2.size()) {
    return false;
  }
  for (size_t i = 0; i < str1.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(str1[i])) !=
        std::tolower(static_cast<unsigned char>(str2[i]))) {
      return false;
    }
  }
  return true;
}

const char* mem_find(const char* src_str, size_t src_len, const char* sub_str, size_t sub_len, bool forward) {
  if (src_str == nullptr || sub_str == nullptr || src_len == 0) {
    return nullptr;
  }

  // sub_len 补全必须在边界检查之前，否则 sub_len==0 时会跳过后续校验
  if (sub_len == 0) {
    sub_len = strlen(sub_str);
  }
  if (sub_len == 0 || sub_len > src_len) {
    return nullptr;
  }

  const size_t last = src_len - sub_len;
  for (size_t i = 0; i <= last; i++) {
    const size_t pos = forward ? i : last - i;
    if (memcmp(src_str + pos, sub_str, sub_len) == 0) {
      return src_str + pos;
    }
  }
  return nullptr;
}

std::string int2string(uint32_t user_id) {
  std::stringstream ss;
  ss << user_id;
  return ss.str();
}

uint32_t string2int(const std::string& value) {
  return (uint32_t)atoi(value.c_str());
}

void replace_mark(std::string& str, std::string& new_value, uint32_t& begin_pos) {
  std::string::size_type pos = str.find('?', begin_pos);
  if (pos == std::string::npos) {
    return;
  }

  std::string prime_new_value = "'" + new_value + "'";
  str.replace(pos, 1, prime_new_value);

  begin_pos = pos + prime_new_value.size();
}

void replace_mark(std::string& str, uint32_t new_value, uint32_t& begin_pos) {
  std::stringstream ss;
  ss << new_value;

  std::string str_value = ss.str();
  std::string::size_type pos = str.find('?', begin_pos);
  if (pos == std::string::npos) {
    return;
  }

  str.replace(pos, 1, str_value);
  begin_pos = pos + str_value.size();
}

char* replace_str(char* pSrc, char oldChar, char newChar) {
  if (NULL == pSrc) {
    return NULL;
  }
  char* pHead = pSrc;
  while (*pHead != '\0') {
    if (*pHead == oldChar) {
      *pHead = newChar;
    }
    ++pHead;
  }
  return pSrc;
}

void str_explode(const std::string& str, char separator, std::vector<std::string>& out) {
  out.clear();
  std::string::size_type start = 0;
  while (true) {
    auto pos = str.find(separator, start);
    if (pos == std::string::npos) {
      out.emplace_back(str.substr(start));
      break;
    }
    out.emplace_back(str.substr(start, pos - start));
    start = pos + 1;
  }
}

}  // namespace teamtalk::imcore::string