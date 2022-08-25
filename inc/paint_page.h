#ifndef _EBOOK_CORE_PAINT_PAGE_H_
#define _EBOOK_CORE_PAINT_PAGE_H_

#include "paint_line.h"

#include <vector>

class paint_page
{
public:
    paint_page(SkScalar max_width, SkScalar max_height);
    ~paint_page();

public:
    bool add(paint_line* block, SkScalar offsetY);
    void paint(const SkPoint& point, SkCanvas* canvas);
    bool empty() const;
    int32_t get_start_pos() const;
    int32_t get_end_pos() const;
    int32_t get_address(const SkPoint& point) const;

protected:
    SkScalar max_width_;
    SkScalar max_height_;

    int32_t start_pos_ = -1;
    int32_t end_pos_ = -1;

    std::vector<std::pair<SkPoint, paint_line*>> lines_;
};

#endif