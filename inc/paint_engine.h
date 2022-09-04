#ifndef _EBOOK_CORE_PAINT_ENGINE_H_
#define _EBOOK_CORE_PAINT_ENGINE_H_

#include "paint_block.h"
#include "paint_page.h"

#include "include/core/SkImage.h"

#include <vector>

class mobi_page;

class paint_engine
{
public:
    paint_engine(SkScalar max_width, SkScalar max_height, const mobi_page* page);
    paint_engine(SkScalar max_width, SkScalar max_height, sk_sp<SkImage> image);
    ~paint_engine();

public:
    int32_t get_page_count() const;
    paint_page* get_page(int32_t index) const;
    int32_t get_page_index(int32_t pos) const;

protected:
    void clear();
    void init(const mobi_page* page);
    // bool traversal_callback(mobi_element* element, int32_t index, int32_t count);

protected:
    SkScalar max_width_;
    SkScalar max_height_;

    std::vector<paint_block*> blocks_;
    std::vector<paint_page*> pages_;
};

#endif