/**
 * @author: luochenhao
 * @email: lch2022fox@163.com
 * @time: Fri 01 May 2026 21:41:56 CST
 * @brief: 字符串相关函数
 */

#ifndef TEAMTALK_IMCORE_STRING_STRING_H_
#define TEAMTALK_IMCORE_STRING_STRING_H_

#include <cctype>
#include <string>
#include <vector>

namespace teamtalk::imcore::string {

/// @brief 将数字（0-15）转换为对应的大写十六进制字符
inline unsigned char to_hex(unsigned char x) {
  return x > 9 ? x - 10 + 'A' : x + '0';
}

/// @brief 将大写十六进制字符转换为对应的数值（0-15）
inline unsigned char from_hex(unsigned char x) {
  return isdigit(x) ? x - '0' : x - 'A' + 10;
}

/// @brief 对字符串进行 URL 编码
std::string url_encode(const std::string& s);

/// @brief 对 URL 编码的字符串进行解码
std::string url_decode(const std::string& s);

/// @brief 去除字符串首尾的空白字符（空格、制表符、回车、换行），原地修改
void str_trim(std::string& s);

/// @brief 安全地将字符串解析为整数
/// @param s   待解析的字符串
/// @param out 解析成功时写入结果
/// @return    解析成功返回 true，字符串非法或溢出返回 false
bool str_to_int(const std::string& s, int& out);

/// @brief 忽略大小写比较两个字符串是否完全相等
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @return 完全相等（忽略大小写）返回 true
bool str_iequals(const std::string& str1, const std::string& str2);

/// @brief 在内存块中查找子串
/// @param src_str 源内存起始地址
/// @param src_len 源内存长度
/// @param sub_str 子串起始地址
/// @param sub_len 子串长度（传 0 则由 strlen 自动计算）
/// @param forward true 从左向右搜索，false 从右向左搜索
/// @return 找到返回匹配位置指针，未找到返回 nullptr
const char* mem_find(const char* src_str, size_t src_len, const char* sub_str, size_t sub_len, bool forward = true);

/// @brief 将无符号整数转为十进制字符串
/// @param user_id 用户 ID 或其它数值
/// @return 十进制文本
std::string int2string(uint32_t value);

/// @brief 将字符串解析为无符号整数（atoi 语义）
/// @param value 输入字符串
/// @return 转换结果，非法前缀按 atoi 规则处理
uint32_t string2int(const std::string& value);

/// @brief 将 str 中从 begin_pos 起第一个 `?` 替换为带单引号的 new_value
/// @param str 被修改的字符串
/// @param new_value 替换内容（外围会加上单引号）
/// @param begin_pos 查找起始下标，成功后会更新为替换段末尾之后的位置
void replace_mark(std::string& str, std::string& new_value, uint32_t& begin_pos);

/// @brief 将 str 中从 begin_pos 起第一个 `?` 替换为 new_value 的十进制文本
/// @param str 被修改的字符串
/// @param new_value 数值，会格式化为无引号十进制串
/// @param begin_pos 查找起始下标，成功后会更新为替换段末尾之后的位置
void replace_mark(std::string& str, uint32_t new_value, uint32_t& begin_pos);

/// @brief 将 pSrc 中所有 oldChar 替换为 newChar（原地修改）
/// @param pSrc 以空字符结尾的缓冲区，可为 nullptr
/// @param oldChar 待替换字符
/// @param newChar 新字符
/// @return pSrc；若 pSrc 为 nullptr 则返回 nullptr
char* replace_str(char* pSrc, char oldChar, char newChar);

/// @brief 按指定分隔符将字符串拆分为子串数组
/// @param str       待拆分的字符串
/// @param separator 分隔符
/// @param out       拆分后的子串，传入前会被清空
void str_explode(const std::string& str, char separator, std::vector<std::string>& out);

}  // namespace teamtalk::imcore::string

#endif  // TEAMTALK_IMCORE_STRING_STRING_H_