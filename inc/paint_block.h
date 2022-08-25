#ifndef _EBOOK_CORE_PAINT_BLOCK_H_
#define _EBOOK_CORE_PAINT_BLOCK_H_

#include "mobi_attr.h"

// Skia
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImage.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkSize.h"


#include <stdint.h>
#include <string>

class mobi_element_image;
class mobi_element_text;
class mobi_element;

class paint_block
{
public:
    paint_block() = default;
    virtual ~paint_block() = default;

public:
    virtual bool is_vilad() const = 0;
    virtual SkRect get_rect() const = 0;
    virtual void paint(const SkPoint& point, SkCanvas* canvas) const = 0;
    virtual void paint(SkScalar x, SkScalar y, SkCanvas* canvas) const;

public:
    mobi_attr::h_align_type get_h_align() const;
    mobi_attr::v_align_type get_v_align() const;
    int32_t get_start_pos() const;
    int32_t get_end_pos() const;
    int32_t get_address() const;

protected:
    mobi_attr::h_align_type h_align_ = mobi_attr::h_align_type::left;
    mobi_attr::v_align_type v_align_ = mobi_attr::v_align_type::baseline;

    int32_t start_pos_ = 0;
    int32_t end_pos_ = 0;

    int32_t address_ = -1;
};

class paint_none_block: public paint_block
{
public:
    paint_none_block() = default;
    virtual ~paint_none_block() = default;

public:
    static paint_none_block* create(const mobi_element* element);

public:
    bool is_vilad() const override;
    SkRect get_rect() const override;
    void paint(const SkPoint& point, SkCanvas* canvas) const override;
};

class paint_blob_block: public paint_block
{
public:
    paint_blob_block() = default;
    virtual ~paint_blob_block() = default;

public:
    static paint_blob_block* create(const mobi_element_text* text, int32_t index, int32_t max_width, int32_t* end_pos, bool* newline);

public:
    bool is_vilad() const override;
    SkRect get_rect() const override;
    void paint(const SkPoint& point, SkCanvas* canvas) const override;

private:
    sk_sp<SkTextBlob> blob_;
    SkColor color_;
};

class paint_image_block: public paint_block
{
public:
    paint_image_block() = default;
    virtual ~paint_image_block() = default;

public:
    static paint_image_block* create(const mobi_element_image* image, int32_t max_width, int32_t max_height);

public:
    bool is_vilad() const override;
    SkRect get_rect() const override;
    void paint(const SkPoint& point, SkCanvas* canvas) const override;

private:
    sk_sp<SkImage> image_;
    SkSize size_;
};

#endif