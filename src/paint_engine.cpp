#include "paint_engine.h"

#include "mobi_element_br.h"
#include "mobi_element_image.h"
#include "mobi_element_paragraph.h"
#include "mobi_element_text.h"
#include "mobi_element.h"
#include "mobi_page.h"

const SkScalar g_span = 4.0;

paint_engine::paint_engine(SkScalar max_width, SkScalar max_height, const mobi_page* page)
    : max_width_(max_width),
      max_height_(max_height)
{
    init(page);
}

paint_engine::~paint_engine()
{
    clear();
}

int32_t paint_engine::get_page_count() const
{
    return pages_.size();
}

paint_page* paint_engine::get_page(int32_t index) const
{
    if (index < 0 || index >= pages_.size()) return NULL;
    return pages_[index];
}

int32_t paint_engine::get_page_index(int32_t pos) const
{
    // 因为没有终止位, 使用这个方法如果 pos的位置在pages_中所有实例的后面，这个结果不符合预期
    // 调用者确保不发生这个情况
    int32_t result = -1;
    for (int32_t index = 0; index < pages_.size() ; ++index) {
        if (pos >= pages_[index]->get_start_pos() && pos < pages_[index]->get_end_pos()) {
            result = index;
            break;
        }
    }
    return result;
}

void paint_engine::clear()
{
    int32_t index = 0;
    for (index = 0; index < pages_.size(); ++index) {
        delete pages_[index];
    }
    pages_.clear();
    for (index = 0; index < blocks_.size(); ++index) {
        delete blocks_[index];
    }
    blocks_.clear();
}

void paint_engine::init(const mobi_page* book_page)
{
    clear();

    paint_page* page = new paint_page(max_width_, max_height_);
    paint_line* line = new paint_line(max_width_);
    SkScalar y = 0.0;

    auto proc = [&](mobi_element* element, int32_t index, int32_t count) -> bool
    {
        switch(element->get_type())
        {
        case mobi_element::mobi_element_type::text:
            {
                int32_t pos = 0;
                while(pos >= 0) {
                    int32_t end_pos = 0;
                    bool newline= false;
                    paint_blob_block* block = paint_blob_block::create((const mobi_element_text*)(element), pos, SkScalarFloorToInt(max_width_ - line->get_width()), &end_pos, &newline);
                    if (block) {
                        blocks_.push_back(block);
                        line->add(block);
                    }
                    if (newline) {
                        if (!page->add(line, y)) {
                            pages_.push_back(page);
                            page = new paint_page(max_width_, max_height_);
                            y = g_span;
                            page->add(line, y);
                        }
                        y += line->get_height() + g_span;
                        line = new paint_line(max_width_);
                    }
                    pos = end_pos;
                }
            }
            break;
        case mobi_element::mobi_element_type::image:
            {
                paint_image_block* block = paint_image_block::create((const mobi_element_image*)(element),
                    SkScalarFloorToInt(max_width_ - line->get_width()),
                    SkScalarFloorToInt(max_height_ - y));
                if (!block) {
                    if (!line->empty()) {
                        if (!page->add(line, y)) {
                            pages_.push_back(page);
                            page = new paint_page(max_width_, max_height_);
                            y = 0.0;
                            page->add(line, y);
                        }
                        y += line->get_height() + g_span;
                        line = new paint_line(max_width_);
                    }
                    block = paint_image_block::create((const mobi_element_image*)(element),
                        SkScalarFloorToInt(max_width_ - line->get_width()),
                        SkScalarFloorToInt(max_height_ - y));
                }
                if (!block) {
                    if (!page->empty()) {
                        pages_.push_back(page);
                        page = new paint_page(max_width_, max_height_);
                        y = 0.0;
                    }
                    block = paint_image_block::create((const mobi_element_image*)(element),
                        SkScalarFloorToInt(max_width_ - line->get_width()),
                        SkScalarFloorToInt(max_height_ - y));
                }
                if (block) {
                    blocks_.push_back(block);
                    line->add(block);
                }
            }
            break;
        case mobi_element::mobi_element_type::br:
            line->add(element);
            if (!line->empty()) {
                if (!page->add(line, y)) {
                    pages_.push_back(page);
                    page = new paint_page(max_width_, max_height_);
                    y = 0.0;
                    page->add(line, y);
                }
                y += line->get_height() + g_span;
                line = new paint_line(max_width_);
            }
            y += 15.0;
            if (y > max_height_) {
                pages_.push_back(page);
                page = new paint_page(max_width_, max_height_);
                y = 0.0;
            }
            break;
        case mobi_element::mobi_element_type::paragraph:
            line->add(element);
            if (!line->empty()) {
                if (!page->add(line, y)) {
                    pages_.push_back(page);
                    page = new paint_page(max_width_, max_height_);
                    y = 0.0;
                    page->add(line, y);
                }
                y += line->get_height() + g_span;
                line = new paint_line(max_width_);
            }
            y += 10.0;
            if (y > max_height_) {
                pages_.push_back(page);
                page = new paint_page(max_width_, max_height_);
                y = 0.0;
            }
            break;
            default:
                break;
        }
        return true;
    };
    book_page->traversal(proc);

    if (!line->empty()) {
        if (!page->add(line, y)) {
            pages_.push_back(page);
            page = new paint_page(max_width_, max_height_);
            y = 0.0;
            page->add(line, y);
        }
    }
    if (!page->empty()) {
        pages_.push_back(page);
    }
    // page->traversal(std::bind(&paint_engine::traversal_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

// bool paint_engine::traversal_callback(mobi_element* element, int32_t index, int32_t count)
// {
//     return true;
// }
