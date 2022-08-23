#ifndef _EBOOK_CORE__CONV_CODE_H_
#define _EBOOK_CORE__CONV_CODE_H_

#include <string>

bool cp1252_to_utf8(const char* buf, int len, std::string& text);

#endif