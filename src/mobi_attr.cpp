#include "mobi_attr.h"

#include "xml_util.h"

#include <strings.h>

mobi_attr::mobi_attr(int32_t pos, const char* key, const char* v)
    : pos_(pos),
      type_(mobi_attr::mobi_attr_type::invilad),
      value_(0)
{
    parse(key, v);
}

mobi_attr::mobi_attr(int32_t pos, mobi_attr_type type, int32_t value)
    : pos_(pos),
      type_(type),
      value_(value)
{

}

mobi_attr::mobi_attr(mobi_attr_type type, int32_t value)
    : pos_(-1),
      type_(type),
      value_(value)
{

}

mobi_attr::mobi_attr(mobi_attr_type type, const std::string& value)
    : pos_(-1),
      type_(type),
      str_value_(value),
      value_(0)
{

}

mobi_attr::mobi_attr(const mobi_attr& attr)
{
    pos_ = attr.pos_;
    type_ = attr.type_;
    value_ = attr.value_;
    str_value_ = attr.str_value_;
}

mobi_attr::~mobi_attr()
{

}

int32_t mobi_attr::get_pos() const
{
    return pos_;
}

mobi_attr::mobi_attr_type mobi_attr::get_type() const
{
    return type_;
}

int32_t mobi_attr::get_value() const
{
    return value_;
}

const std::string& mobi_attr::get_string() const
{
    return str_value_;
}

void mobi_attr::parse(const char* key, const char* v)
{
    if (!strcasecmp(key, "align")) {
        if (!strcasecmp(v, "left")) {
            type_ = mobi_attr::mobi_attr_type::h_align;
            value_ = mobi_attr::h_align_type::left;
        } else if (!strcasecmp(v, "right")) {
            type_ = mobi_attr::mobi_attr_type::h_align;
            value_ = mobi_attr::h_align_type::right;
        } else if (!strcasecmp(v, "center")) {
            type_ = mobi_attr::mobi_attr_type::h_align;
            value_ = mobi_attr::h_align_type::center;
        } else if (!strcasecmp(v, "top")) {
            type_ = mobi_attr::mobi_attr_type::v_align;
            value_ = mobi_attr::v_align_type::top;
        } else if (!strcasecmp(v, "bottom")) {
            type_ = mobi_attr::mobi_attr_type::v_align;
            value_ = mobi_attr::v_align_type::bottom;
        } else if (!strcasecmp(v, "baseline")) {
            type_ = mobi_attr::mobi_attr_type::v_align;
            value_ = mobi_attr::v_align_type::baseline;
        }
    }
    else if (!strcasecmp(key, "font_size")) {
        int32_t value = 0;
        if (StringToNumber(v, value)) {
            type_ = mobi_attr::mobi_attr_type::font_size;
            if (value <= 0) {
                value = 3;
            } else if (value > 7) {
                value = 7;
            }
            value_ = value;
        }
    }
    else if (!strcasecmp(key, "font_name")) {
        type_ = mobi_attr::mobi_attr_type::font_name;
        value_ = 0;
        str_value_ = v;
    }
    else if (!strcasecmp(key, "color")) {

    }
    else if (!strcasecmp(key, "filepos")) {
        int32_t value = 0;
        if (StringToNumber(v, value)) {
            type_ = mobi_attr::mobi_attr_type::link;
            value_ = value;
        }
    }
}
