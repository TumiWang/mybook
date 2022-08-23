#include "mobi_element_image.h"

// Skia
#include "include/core/SkBitmap.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkData.h"
#include "include/core/SkImageEncoder.h"

mobi_element_image::mobi_element_image(const std::vector<char>& data)
{
    sk_sp<SkData> image_data =  SkData::MakeWithoutCopy(&(data[0]), data.size());
    image_ = SkImage::MakeFromEncoded(image_data);
}

mobi_element_image::~mobi_element_image()
{

}

mobi_element::mobi_element_type mobi_element_image::get_type() const
{
    return mobi_element::image;
}

sk_sp<SkImage> mobi_element_image::get_image() const
{
    return image_;
}

int32_t mobi_element_image::get_image_width() const
{
    if (image_) {
        if (width_ > 0) return width_;
        return image_->width();
    }
    return 0;
}

int32_t mobi_element_image::get_image_height() const
{
    if (image_) {
        if (height_ > 0) return height_;
        return image_->height();
    }
    return 0;
}
