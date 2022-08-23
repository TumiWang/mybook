#ifndef _EBOOK_CORE_MOBI_ATTR_H_
#define _EBOOK_CORE_MOBI_ATTR_H_

#include <stdint.h>
#include <string>

class mobi_attr
{
public:
    enum mobi_attr_type
    {
        invilad = 0,
        font_size,
        font_name,
        bold,
        italic,
        width,
        height,
        color,
        h_align,
        v_align,
        link
    };
    enum h_align_type
    {
        left = 0,
        center,
        right
    };
    enum v_align_type
    {
        top = 0,
        baseline,
        bottom
    };

public:
    mobi_attr(int32_t pos, const char* key, const char* v);
    mobi_attr(int32_t pos, mobi_attr_type type, int32_t value);
    mobi_attr(mobi_attr_type type, int32_t value);
    mobi_attr(mobi_attr_type type, const std::string& value);
    mobi_attr(const mobi_attr& attr);
    ~mobi_attr();

public:
    int32_t get_pos() const;
    mobi_attr_type get_type() const;
    int32_t get_value() const;
    const std::string& get_string() const;

protected:
    void parse(const char* key, const char* v);

private:
    int32_t pos_;
    mobi_attr_type type_;
    int32_t value_;
    std::string str_value_;
};

#endif