#ifndef _EBOOK_CORE_MOBI_PAGE_H_
#define _EBOOK_CORE_MOBI_PAGE_H_

#include <stdint.h>

#include "mobi_element.h"

#include <functional>

class mobi_page
{
public:
    mobi_page();
    ~mobi_page();

public:
    int32_t get_start_pos() const;
    bool traversal(const std::function<bool(mobi_element*, int32_t index, int32_t count)>& proc) const;

protected:
    std::vector<mobi_element*> children_;

private:
    friend class mobi_book_parser;
};

#endif