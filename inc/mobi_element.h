#ifndef _EBOOK_CORE_MOBI_ELEMENT_H_
#define _EBOOK_CORE_MOBI_ELEMENT_H_

#include "mobi_attr.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"

#include <vector>
#include <stdint.h>

class mobi_element
{
public:
    enum mobi_element_type
    {
        text,
        image,
        br,
        paragraph
    };

public:
    mobi_element();
    virtual ~mobi_element();

public:
    virtual mobi_element_type get_type() const = 0;

    int32_t get_start_pos() const;

    const std::string& get_font_name() const;
    bool get_address(int32_t* addr) const;
    int32_t get_font_size() const;
    bool is_bold() const;
    bool is_italic() const;
    int32_t get_color() const;
    mobi_attr::h_align_type get_h_align() const;
    mobi_attr::v_align_type get_v_align() const;

protected:
    int32_t start_pos_;

    std::string font_name_;
    int32_t* address_ = NULL;
    int32_t* font_size_ = NULL;
    bool* bold_ = NULL;
    bool* italic_ = NULL;
    int32_t* color_ = NULL;
    mobi_attr::h_align_type* h_align_ = NULL;
    mobi_attr::v_align_type* v_align_ = NULL;

private:
    friend class mobi_book_parser;
};

#endif