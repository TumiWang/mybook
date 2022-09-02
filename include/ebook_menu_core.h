#ifndef _EBOOK_CORE_BOOK_MENU_CORE_H_
#define _EBOOK_CORE_BOOK_MENU_CORE_H_

#include <string>

class book_menu_core
{
public:
    book_menu_core() = default;
    ~book_menu_core() = default;

public:
    virtual int32_t count() const = 0;
    virtual std::string get_title(int32_t index) const = 0;

    virtual int32_t get_addr(int32_t index) const = 0;
    virtual book_menu_core* get_submenu(int32_t index) = 0;
};

#endif