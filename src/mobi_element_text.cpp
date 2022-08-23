#include "mobi_element_text.h"

mobi_element_text::mobi_element_text(const char* buf, int32_t len)
{
    content_ = std::string(buf, len);
}

mobi_element_text::mobi_element_text(const char* buf)
{
    content_ = buf;
}

mobi_element_text::~mobi_element_text()
{

}

mobi_element::mobi_element_type mobi_element_text::get_type() const
{
    return mobi_element::text;
}

std::string mobi_element_text::get_content() const
{
    return content_;
}
