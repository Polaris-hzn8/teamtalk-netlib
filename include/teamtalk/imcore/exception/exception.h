
/*
 Reviser: Polaris_hzn8
 Email: lch2022fox@163.com
 Github: https://github.com/Polaris-hzn8
 brief: 逻辑引擎异常
*/

#ifndef TEAMTALK_IMCORE_EXCEPTION_EXCEPTION_H_
#define TEAMTALK_IMCORE_EXCEPTION_EXCEPTION_H_

#include <string>

namespace teamtalk::imcore::exception {

class Exception : public std::exception {
 public:
  Exception(int32_t code, const std::string& msg) : code_(code), msg_(msg) {}
  virtual ~Exception() = default;

  const char* what() const noexcept override { return msg_.c_str(); }
  int32_t code() const noexcept { return code_; }

 protected:
  int32_t code_;
  std::string msg_;
};

}  // namespace teamtalk::imcore::exception

#endif  // TEAMTALK_IMCORE_EXCEPTION_EXCEPTION_H_
