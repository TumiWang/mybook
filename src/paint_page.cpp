#include "paint_page.h"

paint_page::paint_page(SkScalar max_width, SkScalar max_height)
    : max_width_(max_width),
      max_height_(max_height)
{

}

paint_page::~paint_page()
{
    lines_.clear();
}

bool paint_page::add(paint_line* line, SkScalar offsetY)
{
    SkScalar y = 0.0;
    if (!lines_.empty()) {
        // y = lines_.back().first.fY + lines_.back().second->get_height();
        y = lines_.back().second->get_height();
    }
    y += offsetY;
    if (y + line->get_height() > max_height_) return false;
    lines_.emplace_back(SkPoint::Make(0.0, y), line);
    update_start_pos(line->get_start_pos());
    return true;
}

void paint_page::paint(const SkPoint& point, SkCanvas* canvas)
{
    for (const auto& item: lines_) {
        SkPoint pt = item.first;
        pt.offset(point.fX, point.fY);
        item.second->paint(pt, canvas);
    }
}

bool paint_page::empty() const
{
    return lines_.empty();
}

int32_t paint_page::get_start_pos() const
{
    if (start_pos_ >= 0) return start_pos_;
    if (lines_.empty()) return -1;
    return lines_.front().second->get_start_pos();
}

void paint_page::update_start_pos(int32_t pos)
{
    if (start_pos_ < 0) {
        start_pos_ = pos;
    }
}

int32_t paint_page::get_address(const SkPoint& point) const
{
    int32_t result = -1;
    for (const auto& line: lines_) {
        if (point.fY >= line.first.fY && point.fY < line.first.fY + line.second->get_height()) {
            result = line.second->get_address(SkPoint::Make(point.fX, point.fY - line.first.fY));
            break;
        }
    }
    return result;
}
