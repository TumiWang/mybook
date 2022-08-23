#ifndef _EBOOK_CORE_MOBI_ELEMENT_IMAGE_H_
#define _EBOOK_CORE_MOBI_ELEMENT_IMAGE_H_

#include "mobi_element.h"

// Skia
#include "include/core/SkImage.h"

#include <vector>

class mobi_element_image: public mobi_element
{
public:
    mobi_element_image(const std::vector<char>& data);
    virtual ~mobi_element_image();

public:
    mobi_element_type get_type() const override;

public:
    sk_sp<SkImage> get_image() const;
    int32_t get_image_width() const;
    int32_t get_image_height() const;

protected:
    sk_sp<SkImage> image_;

    int32_t width_ = 0;
    int32_t height_ = 0;

private:
    friend class mobi_book_parser;
};

#endif