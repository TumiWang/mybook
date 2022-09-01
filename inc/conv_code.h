#ifndef _EBOOK_CORE__CONV_CODE_H_
#define _EBOOK_CORE__CONV_CODE_H_

#include <string>

bool cp1252_to_utf8(const char* buf, int len, std::string& text);

bool StringToNumber(const std::string& text, int32_t& v);

std::string GetFontnameFromLanguage(const std::string& language);

#endif