#include "paint_block.h"

#include "mobi_element_text.h"
#include "mobi_element_image.h"

// Skia
#include "include/core/SkFontMgr.h"

namespace
{
    SkScalar get_font_size(int32_t index, int32_t grade)
    {
        static const std::vector<SkScalar> data = {
            10.0, 12.0, 13.0, 16.0, 18.0, 24.0, 32.0, 48.0
        };
        int32_t pos = index + grade;
        if (pos < 0) {
            return data.front();
        } else if (pos >= data.size()) {
            return data.back();
        }
        return data[pos];
    }
}

void paint_block::paint(SkScalar x, SkScalar y, SkCanvas* canvas) const
{
    paint(SkPoint::Make(x, y), canvas);
}

mobi_attr::h_align_type paint_block::get_h_align() const
{
    return h_align_;
}

mobi_attr::v_align_type paint_block::get_v_align() const
{
    return v_align_;
}

int32_t paint_block::get_start_pos() const
{
    return start_pos_;
}

int32_t paint_block::get_end_pos() const
{
    return end_pos_;
}

int32_t paint_block::get_address() const
{
    return address_;
}

// static
paint_none_block* paint_none_block::create(const mobi_element* element)
{
    paint_none_block* block = new paint_none_block();
    block->v_align_ = mobi_attr::h_align_type::left;
    block->h_align_ = mobi_attr::v_align_type::top;
    block->start_pos_ = element->get_start_pos();
    block->end_pos_ = element->get_end_pos();
    return block;
}

bool paint_none_block::is_vilad() const
{
    return true;
}

SkRect paint_none_block::get_rect() const
{
    return SkRect::MakeEmpty();
}

void paint_none_block::paint(const SkPoint& point, SkCanvas* canvas) const
{
}

// static
paint_blob_block* paint_blob_block::create(const mobi_element_text* text, int32_t index, int32_t max_width, int32_t* end_pos, bool* newline)
{
    paint_blob_block* block = new paint_blob_block();
    std::string content = text->get_content();
    block->v_align_ = text->get_v_align();
    block->h_align_ = text->get_h_align();
    block->color_ = text->get_color();
    block->start_pos_ = text->get_start_pos() + index;
    int32_t address = -1;
    if (text->get_address(&address)) block->address_ = address;

    sk_sp<SkFontMgr> font_mgr = SkFontMgr::RefDefault();
    sk_sp<SkTypeface> typeface;
    std::string font_name = text->get_font_name();
    if (text->is_bold()) {
        if (text->is_italic()) {
            typeface = font_mgr->legacyMakeTypeface(font_name.c_str(), SkFontStyle::BoldItalic());
        } else {
            typeface = font_mgr->legacyMakeTypeface(font_name.c_str(), SkFontStyle::Bold());
        }
    } else {
        if (text->is_italic()) {
            typeface = font_mgr->legacyMakeTypeface(font_name.c_str(), SkFontStyle::Italic());
        } else {
            typeface = font_mgr->legacyMakeTypeface(font_name.c_str(), SkFontStyle::Normal());
        }
    }
    SkFont font(typeface, get_font_size(text->get_font_size(), 1));
    font.setEdging(SkFont::Edging::kAntiAlias);

    int size = 0;
    bool is_newline = false;
    int current_size = 0;
    while (true) {
        ++size;
        if (index + size > content.size()) {
            current_size = -1;
            break;
        }
        std::string substr = content.substr(index, size);
        sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(substr.c_str(), substr.size(), font);
        if (blob.get() == NULL) continue;
        if (max_width > 0 && blob->bounds().width() >= max_width) {
            is_newline = true;
            break;
        }
        current_size = size;
        block->blob_ = blob;
    }
    if (end_pos) {
        if (current_size < 0) *end_pos = -1;
        else *end_pos = index + current_size;
    }
    if (current_size < 0) block->end_pos_ = text->get_end_pos();
    else block->end_pos_ = block->start_pos_ + current_size;

    if (newline) *newline = is_newline;
    if (!block->is_vilad()) {
        delete block;
        block = NULL;
    }

    return block;
}

bool paint_blob_block::is_vilad() const
{
    return !!blob_;
}

SkRect paint_blob_block::get_rect() const
{
    if (blob_) return blob_->bounds();
    return SkRect::MakeEmpty();
}

void paint_blob_block::paint(const SkPoint& point, SkCanvas* canvas) const
{
    SkPaint paint;
    paint.setColor(color_);
    paint.setAntiAlias(true);
    // paint.setDither(true);
    // paint.setFilterQuality(kHigh_SkFilterQuality);
    canvas->drawTextBlob(blob_, point.fX, point.fY, paint);
}

// static
paint_image_block* paint_image_block::create(const mobi_element_image* image, int32_t max_width, int32_t max_height)
{
    static int32_t iii = 500;
    paint_image_block* block = new paint_image_block();
    block->v_align_ = image->get_v_align();
    block->h_align_ = image->get_h_align();
    block->image_ = image->get_image();
    block->start_pos_ = image->get_start_pos();
    block->end_pos_ = image->get_end_pos();
    int32_t address = -1;
    if (image->get_address(&address)) block->address_ = address;

    int32_t w = image->get_image_width();
    int32_t h = image->get_image_height();
    if (w > max_width) w = max_width;
    if (h > max_height) h = max_height;
    block->size_ = SkSize::Make(SkIntToScalar(w), SkIntToScalar(h));
    if (!block->is_vilad()) {
        delete block;
        block = NULL;
    }
    return block;
}

bool paint_image_block::is_vilad() const
{
    return !!image_;
}

SkRect paint_image_block::get_rect() const
{
    if (image_) {
        return SkRect::MakeSize(size_);
    }
    return SkRect::MakeEmpty();
}

void paint_image_block::paint(const SkPoint& point, SkCanvas* canvas) const
{
    SkSamplingOptions options;
    canvas->drawImageRect(image_,
        SkRect::MakeIWH(image_->width(), image_->height()),
        SkRect::MakeXYWH(point.fX,point.fY, size_.fWidth, size_.fHeight),
        options, nullptr, SkCanvas::kFast_SrcRectConstraint);
}
