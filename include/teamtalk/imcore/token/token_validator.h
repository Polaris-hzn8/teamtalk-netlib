/*
 Reviser: Polaris_hzn8
 Email: 3453851623@qq.com
 filename: TokenValidator.h
 Update Time: Tue 13 Jun 2023 17:09:29 CST
 brief:
*/

#ifndef TEAMTALK_IMCORE_TOKEN_TOKEN_VALIDATOR_H_
#define TEAMTALK_IMCORE_TOKEN_TOKEN_VALIDATOR_H_

#include <time.h>
#include <stdio.h>
#include <stdint.h>

namespace teamtalk::imcore::token {

// 生成令牌token
int genToken(unsigned int uid, time_t time_offset, char* md5_str_buf);

// 验证令牌token
bool IsTokenValid(uint32_t user_id, const char* token);

}  // namespace teamtalk::imcore::token

#endif  // TEAMTALK_IMCORE_TOKEN_TOKEN_VALIDATOR_H_
