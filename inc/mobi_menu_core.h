#ifndef _EBOOK_CORE_MOBI_MENU_H_
#define _EBOOK_CORE_MOBI_MENU_H_

#include <stdint.h>
#include <vector>

#include "ebook_menu_core.h"

#include "mobi_element.h"
#include "paint_block.h"

class mobi_menu_core;

class menu_core_item
{
public:
    menu_core_item() = default;
    ~menu_core_item();

public:
    std::string title_;
    int32_t addr_ = -1;
    mobi_menu_core* submenu_ = NULL;
}

class mobi_menu_core: public book_menu_core
{
public:
    mobi_menu_core() = default;
    ~mobi_menu_core();

// book_menu_core impl
public:
    int32_t count() const override;
    std::string get_title(int32_t index) const override;

    int32_t get_addr(int32_t index) const override;
    book_menu_core* get_submenu(int32_t index) override;

public:
    void addMenuItem(const std::string& title, int32_t addr);
    mobi_menu_core* addSubMenu(const std::string& title);
    void removeSubMenu(mobi_menu_core* submenu);

protected:
    std::vector<menu_core_item*> items_;
};

#endif