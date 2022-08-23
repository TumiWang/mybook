#include "mobi_menu.h"

mobi_menu::mobi_menu()
{

}

mobi_menu::~mobi_menu()
{
    for (auto& item: children_)
    {
        if (item.first) delete item.second;
    }
}

void mobi_menu::create_paint_block()
{
    cleanup_paint_block();

    for (auto& item: children_) {
        height_ += span_;
        mobi_element::mobi_element_type type = item.second->get_type();
        if (type != mobi_element::mobi_element_type::text) continue;
        mobi_element_text* text = (mobi_element_text*)(item.second);
        paint_blob_block* block = paint_blob_block::create(text, 0, -1, NULL, NULL);
        if (!block) continue;
        SkRect rect = block->get_rect();
        SkScalar width = rect.width();
        SkScalar height = rect.height();
        SkScalar ascent = 0.0;
        if (rect.fTop < 0) ascent = SkScalarFloorToInt(SkScalarAbs(rect.fTop));
        blocks_.emplace_back(SkPoint::Make(0.0, height_ + ascent), block);
        if (width_ < width) width_ = width;
        height_ += span_ + height;
    }
    for (auto& block: blocks_) {
        switch (block.second->get_h_align())
        {
        case mobi_attr::h_align_type::right:
            block.first.offset(width_ - block.second->get_rect().width(), 0.0);
            break;
        case mobi_attr::h_align_type::center:
            block.first.offset((width_ - block.second->get_rect().width()) / 2, 0.0);
            break;
        case mobi_attr::h_align_type::left:
        default:
            break;
        }
    }
}

void mobi_menu::paint(const SkPoint& point, SkCanvas* canvas) const
{
    for (const auto& block: blocks_) {
        SkPoint pt = block.first;
        pt.offset(point.fX, point.fY);
        block.second->paint(pt, canvas);
    }
}

int32_t mobi_menu::get_address(const SkPoint& point) const
{
    int32_t result = -1;
    for (const auto& block: blocks_) {
        SkScalar x = block.first.fX - point.fX;
        SkScalar y = block.first.fY - point.fY;
        if (block.second->get_rect().contains(x,y)) {
            result = block.second->get_address();
            break;
        }
    }
    return result;
}

void mobi_menu::cleanup_paint_block()
{
    width_ = 0.0;
    height_ = 0.0;

    for (auto& block: blocks_) {
        if (block.second) delete block.second;
    }
    blocks_.clear();
}
