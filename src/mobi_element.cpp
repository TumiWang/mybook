#include "mobi_element.h"

mobi_element::mobi_element()
    : start_pos_(-1),
      end_pos_(-1)
{

}

mobi_element::~mobi_element()
{
    if (address_) {
        delete address_;
        address_ = NULL;
    }
    if (bold_) {
        delete bold_;
        bold_ = NULL;
    }
    if (italic_) {
        delete italic_;
        italic_ = NULL;
    }
    if (color_) {
        delete color_;
        color_ = NULL;
    }
    if (h_align_) {
        delete h_align_;
        h_align_ = NULL;
    }
    if (v_align_) {
        delete v_align_;
        v_align_ = NULL;
    }
}

int32_t mobi_element::get_start_pos() const
{
    return start_pos_;
}

int32_t mobi_element::get_end_pos() const
{
    return end_pos_;
}

const std::string& mobi_element::get_font_name() const
{
    return font_name_;
}

bool mobi_element::get_address(int32_t* addr) const
{
    if (!address_) return false;
    if (addr) *addr = *address_;
    return true;
}

int32_t mobi_element::get_font_size() const
{
    return *font_size_;
}

bool mobi_element::is_bold() const
{
    return *bold_;
}

bool mobi_element::is_italic() const
{
    return *italic_;
}

int32_t mobi_element::get_color() const
{
    return *color_;
}

mobi_attr::h_align_type mobi_element::get_h_align() const
{
    return *h_align_;
}

mobi_attr::v_align_type mobi_element::get_v_align() const
{
    return *v_align_;
}
