#ifndef _EBOOK_CORE_MOBI_ELEMENT_BR_H_
#define _EBOOK_CORE_MOBI_ELEMENT_BR_H_

#include "mobi_element.h"

class mobi_element_br: public mobi_element
{
public:
    mobi_element_br();
    virtual ~mobi_element_br();

public:
    mobi_element_type get_type() const override;

private:
    friend class mobi_book_parser;
};

#endif