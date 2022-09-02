#include "mobi_menu_core.h"

menu_core_item::~menu_core_item()
{
    if (submenu_) {
        delete submenu_;
        submenu_ = NULL;
    }
}

mobi_menu_core::~mobi_menu_core()
{
    for (int index = 0; index < items_.size(); ++index) {
        menu_core_item* item = items_[index];
        if (item) {
            delete item;
        }
    }
    items_.clear();
}

int32_t mobi_menu_core::count() const
{
    return items_.size();
}

std::string mobi_menu_core::get_title(int32_t index) const
{
    std::string result;
    do {
        if (index < 0) break;
        if (index >= items_.size()) break;
        result = items_[index]->title_;
    } while(false);
    return result;
}

int32_t mobi_menu_core::get_addr(int32_t index) const
{
    int32_t result = -1;
    do {
        if (index < 0) break;
        if (index >= items_.size()) break;
        result = items_[index]->addr_;
    } while(false);
    return result;
}

book_menu_core* mobi_menu_core::get_submenu(int32_t index)
{
    book_menu_core* result = NULL;
    do {
        if (index < 0) break;
        if (index >= items_.size()) break;
        result = items_[index]->submenu_;
    } while(false);
    return result;
}

void mobi_menu_core::addMenuItem(const std::string& title, int32_t addr)
{
    menu_core_item* item = new menu_core_item();
    if (!item) {
        return;
    }
    item->title_ = title;
    item->addr_ = addr;
    items_.push_back(item);
}

mobi_menu_core* mobi_menu_core::addSubMenu(const std::string& title)
{
    mobi_menu_core* submenu = NULL;
    menu_core_item* item = NULL;

    do {
        submenu = new mobi_menu_core();
        if (!submenu) break;
        item = new menu_core_item();
        if (!item) break;
        item->title_ = title;
        item->submenu_ = submenu;
        items_.push_back(item);
    } while(false);

    if (!item) {
        if (submenu) {
            delete submenu;
            submenu = NULL;
        }
    }
    
    return submenu;
}

void mobi_menu_core::removeSubMenu(mobi_menu_core* submenu)
{
    if (submenu == NULL) return;
    for (auto itor = items_.begin(); itor != items_.end(); ++itor) {
        if (itor->submenu_ == submenu) {
            delete *itor;
            items_.erease(itor);
            break;
        }
    }
}
