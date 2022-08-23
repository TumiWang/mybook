#ifndef _EBOOK_CORE_MOBI_ELEMENT_PARAGRAPH_H_
#define _EBOOK_CORE_MOBI_ELEMENT_PARAGRAPH_H_

#include "mobi_element.h"

#include <vector>

class mobi_element_paragraph: public mobi_element
{
public:
    mobi_element_paragraph();
    virtual ~mobi_element_paragraph();

public:
    mobi_element_type get_type() const override;

// protected:
//     std::vector<mobi_element*> children_;

private:
    friend class mobi_book_parser;
};

#endif