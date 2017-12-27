#pragma once
#include <string>

inline const std::string SERVER_ADDR = "sms.yunpian.com";
inline const std::string PORT = "http";
inline const std::string TEMPLATE_TEXT = u8"【xx科技】您的验证码是";
inline const std::string PRETEXT =
	"POST https://sms.yunpian.com/v2/sms/single_send.json HTTP/1.1\r\n"
	"Host: sms.yunpian.com\r\n";