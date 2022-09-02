#ifndef _EBOOK_CORE_MOBI_MENU_H_
#define _EBOOK_CORE_MOBI_MENU_H_

#include <stdint.h>

#include "mobi_element.h"
#include "mobi_menu_core.h"
#include "paint_block.h"

class mobi_menu
{
public:
    mobi_menu();
    ~mobi_menu();

public:
    void create_paint_block();
    void paint(const SkPoint& point, SkCanvas* canvas) const;
    int32_t get_address(const SkPoint& point) const;
    book_menu_core* get_book_menu_core() const;

protected:
    void cleanup_paint_block();

protected:
    std::vector<std::pair<bool, mobi_element*>> children_;

    std::vector<std::pair<SkPoint, paint_block*>> blocks_;

    SkScalar width_ = 0.0;
    SkScalar height_ = 0.0;

    SkScalar span_ = 4.0;

    mobi_menu_core* mobi_menu_core_ = NULL;

private:
    friend class mobi_book_parser;
};

#endif