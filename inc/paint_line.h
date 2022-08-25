#ifndef _EBOOK_CORE_PAINT_LINE_H_
#define _EBOOK_CORE_PAINT_LINE_H_

#include "paint_block.h"

#include <vector>

class paint_line
{
public:
    paint_line(SkScalar max_width);
    ~paint_line();

public:
    SkScalar get_height() const;
    SkScalar get_width() const;
    bool add(paint_block* block);
    void paint(const SkPoint& point, SkCanvas* canvas);
    bool empty() const;
    int32_t get_start_pos() const;
    int32_t get_end_pos() const;
    int32_t get_address(const SkPoint& point) const;

protected:
    void update_children_position_for_top();
    void update_children_position_for_bottom();
    void update_children_position_for_baseline();

protected:
    SkScalar max_width_;

    int32_t start_pos_ = -1;
    int32_t end_pos_ = -1;

    std::vector<std::pair<SkPoint, paint_block*>> children_;
    SkScalar height_ = 0.0;
    SkScalar width_ = 0.0;
};

#endif