#include "paint_line.h"

paint_line::paint_line(SkScalar max_width)
    : max_width_(max_width)
{
}

paint_line::~paint_line()
{
    children_.clear();
}

SkScalar paint_line::get_height() const
{
    return height_;
}

SkScalar paint_line::get_width() const
{
    return width_;
}

bool paint_line::add(paint_block* block)
{
    SkScalar w = max_width_;
    if (!children_.empty()) {
        const auto& last = children_.back();
        w -= last.first.fX + last.second->get_rect().width();
    }
    if (block->get_rect().width() >= w) return false;
    SkPoint point;
    switch (block->get_h_align())
    {
    case mobi_attr::h_align_type::right:
        point = SkPoint::Make(w - block->get_rect().width(), 0.0);
        break;
    case mobi_attr::h_align_type::center:
        point = SkPoint::Make((w - block->get_rect().width()) / 2, 0.0);
        break;
    case mobi_attr::h_align_type::left:
    default:
        point = SkPoint::Make(w, 0.0);
        break;
    }
    children_.emplace_back(point, block);
    mobi_attr::v_align_type v_align = children_.front().second->get_v_align();
    switch (v_align)
    {
    case mobi_attr::v_align_type::top:
        {
            update_children_position_for_top();
        }
        break;
    case mobi_attr::v_align_type::bottom:
        {
            update_children_position_for_bottom();
        }
        break;
    case mobi_attr::v_align_type::baseline:
    default:
        {
            update_children_position_for_baseline();
        }
        break;
    }
    int32_t start_pos = block->get_start_pos();
    int32_t end_pos = block->get_end_pos();
    if (start_pos_ < 0) {
        start_pos_ = start_pos;
    } else {
        if (start_pos < start_pos_) start_pos_ = start_pos;
    }
    if (end_pos_ < 0) {
        end_pos_ = end_pos;
    } else {
        if (end_pos > end_pos_) end_pos_ = end_pos;
    }
    return true;
}

void paint_line::paint(const SkPoint& point, SkCanvas* canvas)
{
    for (const auto& item: children_) {
        SkPoint pt = item.first;
        pt.offset(point.fX, point.fY);
        item.second->paint(pt, canvas);
    }
}

bool paint_line::empty() const
{
    bool result = true;
    for (const auto& item: children_) {
        if (!item.second->get_rect().isEmpty()) {
            result = false;
            break;
        }
    }
    return result;
}

int32_t paint_line::get_start_pos() const
{
    if (start_pos_ >= 0) return start_pos_;
    if (children_.empty()) return -1;
    return children_.front().second->get_start_pos();
}

int32_t paint_line::get_end_pos() const
{
    if (children_.empty()) return -1;
    return children_.back().second->get_end_pos();
}

int32_t paint_line::get_address(const SkPoint& point) const
{
    int32_t result = -1;
    for (const auto& item: children_) {
        SkScalar x = point.fX - item.first.fY;
        SkScalar y = point.fY - item.first.fY;
        if (item.second->get_rect().contains(x,y)) {
            result = item.second->get_address();
            break;
        }
    }
    return result;
}

void paint_line::update_children_position_for_top()
{
    SkScalar x = 0.0;
    int index = 0;
    for (; index < children_.size(); ++index) {
        if (!children_[index].second->is_vilad()) continue;
        SkRect rect = children_[index].second->get_rect();
        SkScalar y = 0.0;
        SkScalar height = 0.0;
        if (rect.fTop < 0) {
            SkScalar ascent = SkScalarFloorToInt(SkScalarAbs(rect.fTop));
            SkScalar descent = SkScalarCeilToInt(SkScalarAbs(rect.fBottom));
            height = ascent + descent;
            y = ascent;
        } else {
            height = SkScalarCeilToInt(rect.height());
            y = 0;
        }
        switch (children_[index].second->get_h_align())
        {
        case mobi_attr::h_align_type::right:
            // x += (max_width_ - x) - rect.width()
            x = max_width_ - rect.width();
            break;
        case mobi_attr::h_align_type::center:
            x += ((max_width_ - x) - rect.width()) / 2;
            break;
        case mobi_attr::h_align_type::left:
        default:
            break;
        }
        children_[index].first.set(x, y);
        x += rect.width();
        if (height_ < height) height_ = height;
    }
    width_ = x;
}

void paint_line::update_children_position_for_bottom()
{
    SkScalar x = 0.0;
    int index = 0;
    for (; index < children_.size(); ++index) {
        if (!children_[index].second->is_vilad()) continue;
        SkRect rect = children_[index].second->get_rect();
        SkScalar y = 0.0;
        SkScalar height = 0.0;
        if (rect.fTop < 0) {
            SkScalar ascent = SkScalarFloorToInt(SkScalarAbs(rect.fTop));
            SkScalar descent = SkScalarFloorToInt(SkScalarAbs(rect.fBottom));
            height = ascent + descent;
            y = ascent;
        } else {
            height = SkScalarCeilToInt(rect.height());
            y = 0;
        }
        switch (children_[index].second->get_h_align())
        {
        case mobi_attr::h_align_type::right:
            // x += (max_width_ - x) - rect.width()
            x = max_width_ - rect.width();
            break;
        case mobi_attr::h_align_type::center:
            x += ((max_width_ - x) - rect.width()) / 2;
            break;
        case mobi_attr::h_align_type::left:
        default:
            break;
        }
        children_[index].first.set(x, y);
        x += rect.width();
        if (height_ < height) {
            SkScalar temp = height - height_;
            for (int i = 0; i < index; ++i) {
                children_[index].first.offset(0.0, temp);
            }
            height_ = height;
        }
    }
    width_ = x;
}

void paint_line::update_children_position_for_baseline()
{
    SkScalar x = 0.0;
    SkScalar baseline = 0.0;
    int index = 0;
    for (; index < children_.size(); ++index) {
        if (!children_[index].second->is_vilad()) continue;
        SkRect rect = children_[index].second->get_rect();
        SkScalar y = 0.0;
        SkScalar ascent = 0.0;
        SkScalar height = 0.0;
        if (rect.fTop < 0) {
            ascent = SkScalarFloorToInt(SkScalarAbs(rect.fTop));
            SkScalar descent = SkScalarFloorToInt(SkScalarAbs(rect.fBottom));
            height = ascent + descent;
            y = ascent;
        } else {
            height = SkScalarCeilToInt(rect.height());
            ascent = height / 2;
            y = 0;
        }
        switch (children_[index].second->get_h_align())
        {
        case mobi_attr::h_align_type::right:
            // x += (max_width_ - x) - rect.width()
            x = max_width_ - rect.width();
            break;
        case mobi_attr::h_align_type::center:
            x += ((max_width_ - x) - rect.width()) / 2;
            break;
        case mobi_attr::h_align_type::left:
        default:
            break;
        }
        children_[index].first.set(x, y);
        x += rect.width();
        if (baseline < ascent) {
            SkScalar temp = baseline - ascent;
            for (int i = 0; i < index; ++i) {
                children_[index].first.offset(0.0, temp);
            }
            baseline = ascent;
        }
        if (height_ < height) height_ = height;
    }
    width_ = x;
}
