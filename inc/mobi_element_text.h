#ifndef _EBOOK_CORE_MOBI_ELEMENT_TEXT_H_
#define _EBOOK_CORE_MOBI_ELEMENT_TEXT_H_

#include "mobi_element.h"

#include <string>

class mobi_element_text: public mobi_element
{
public:
    mobi_element_text(const char* buf, int32_t len);
    mobi_element_text(const char* buf);
    virtual ~mobi_element_text();

public:
    mobi_element_type get_type() const override;

public:
    std::string get_content() const;

protected:
    std::string content_;

private:
    friend class mobi_book_parser;
};

#endif