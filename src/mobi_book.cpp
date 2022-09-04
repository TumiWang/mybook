#include "mobi_book.h"

mobi_book::mobi_book()
{
    size_ = SkSize::MakeEmpty();
    current_pos_ = -1;
}

mobi_book::~mobi_book()
{
    cleanup_paint_engine();

    if (menu_) {
        delete menu_;
        menu_ = NULL;
    }
    for (auto& item: pages_)
    {
        delete item;
    }
    pages_.clear();
}

bool mobi_book::is_type(const char* type_name) const
{
    const static std::string t = "mobi_book";
    return t == type_name;
}

std::string mobi_book::get_title() const
{
    return title_;
}

void mobi_book::set_size(SkSize size)
{
    if (size_ == size) return;
    size_ = size;
    if (current_pos_ >= 0 && !size_.isEmpty()) {
        create_paint_engine();
    }
}

void mobi_book::SetCurrentPos(int32_t pos)
{
    if (current_pos_ == pos) return;
    current_pos_ = pos;
    if (current_pos_ >= 0 && !size_.isEmpty()) {
        create_paint_engine();
    }
}

int32_t mobi_book::GetCurrentPos() const
{
    return current_pos_;
}

void mobi_book::PrivPage()
{
    std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);

    if (engine_index_ < 0 || engine_index_ >= paint_engine_.size()) return;
    paint_engine* engine = paint_engine_[engine_index_];
    if (!engine) return;
    int32_t page_index = page_index_;
    if (page_index < 0 || page_index >= engine->get_page_count()) {
        page_index = engine->get_page_count() - 1;
    }
    if (page_index - 1 >= 0) {
        page_index_ = page_index - 1;
    } else {
        if (engine_index_ - 1 >= 0) {
            engine = paint_engine_[engine_index_ - 1];
            if (!engine) return;
            engine_index_ -= 1;
            page_index_ = -1;
        }
    }
    update_current_pos();
}

void mobi_book::NextPage()
{
    std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);

    if (engine_index_ < 0 || engine_index_ >= paint_engine_.size()) return;
    paint_engine* engine = paint_engine_[engine_index_];
    if (!engine) return;
    int32_t page_index = page_index_;
    if (page_index < 0 || page_index >= engine->get_page_count()) {
        page_index = engine->get_page_count() - 1;
    }
    if (page_index + 1 < engine->get_page_count()) {
        page_index_ = page_index + 1;
    } else {
        if (engine_index_ + 1 < paint_engine_.size()) {
            engine = paint_engine_[engine_index_ + 1];
            if (!engine) return;
            engine_index_ += 1;
            page_index_ = 0;
        }
    }
    update_current_pos();
}

void mobi_book::ProcClick(const SkPoint& point)
{
    int32_t pos = -1;

    std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);
    
    if (engine_index_ < 0 || engine_index_ >= paint_engine_.size()) return;
    paint_engine* engine = paint_engine_[engine_index_];
    if (!engine) return;
    int32_t page_index = page_index_;
    if (page_index < 0 || page_index >= engine->get_page_count()) {
        page_index = engine->get_page_count() - 1;
    }
    if (page_index < 0) return;
    paint_page* page = engine->get_page(page_index);
    if (!page) return;
    pos = page->get_address(point);
    if (pos < 0) return;

    for (int index = 0; index < paint_engine_.size(); ++index) {
        paint_engine* engine = paint_engine_[index];
        if (!engine) break;
        int32_t temp = engine->get_page_index(pos);
        if (temp >= 0) {
            engine_index_ = index;
            page_index_ = temp;
            break;
        }
    }
    update_current_pos();
}

void mobi_book::paint(const SkPoint& point, SkCanvas* canvas)
{
    std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);
    
    if (engine_index_ < 0 || engine_index_ >= paint_engine_.size()) return;
    paint_engine* engine = paint_engine_[engine_index_];
    if (!engine) return;
    int32_t page_index = page_index_;
    if (page_index < 0 || page_index >= engine->get_page_count()) {
        page_index = engine->get_page_count() - 1;
    }
    if (page_index < 0) return;
    paint_page* page = engine->get_page(page_index);
    if (!page) return;
    page->paint(point, canvas);
}

book_menu_core* mobi_book::get_menu() const
{
    if (!menu_) return NULL;
    return menu_->get_book_menu_core();
}

SkImage* mobi_book::get_cover() const
{
    return cover_image_.get();
}

SkImage* mobi_book::get_thumb() const
{
    return thumb_image_.get();
}

std::string mobi_book::get_attr(book_attr_type attr) const
{
    std::string result;

    for (auto itor = attrs_.begin(); itor != attrs_.end(); ++itor) {
        if (itor->first == attr) {
            result = itor->second;
            break;
        }
    }

    return result;
}

int32_t mobi_book::get_default_pos() const
{
    return defalut_pos_;
}

void mobi_book::cleanup_paint_engine()
{
    std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);

    for (auto& item: paint_engine_) {
        if (item) delete item;
    }
    paint_engine_.clear();

    engine_index_ = 0;
    page_index_ = 0;
}

void mobi_book::create_paint_engine()
{
    cleanup_paint_engine();

    int32_t index_engine = -1;
    int32_t count_engine = 0;
    for (int32_t i = 0; i < pages_.size(); ++i) {
        int32_t temp = pages_[i]->get_start_pos();
        if (temp < 0) continue;
        if (current_pos_ < temp) {
            if (index_engine < 0) index_engine = i;
            break;
        }
        index_engine = i;
    }
    if (index_engine < 0) return;

    {
        std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);

        count_engine = pages_.size();
        paint_engine_.resize(count_engine);

        paint_engine* engine = new paint_engine(size_.fWidth, size_.fHeight, pages_[index_engine]);
        paint_engine_[index_engine] = engine;

        int32_t index_page = -1;
        int32_t new_pos = current_pos_;
        for (int32_t i = 0; i < engine->get_page_count(); ++i) {
            paint_page* page = engine->get_page(i);
            if (!page) continue;
            int32_t temp = page->get_start_pos();
            if (temp < 0) continue;
            if (current_pos_ < temp) 
            {
                if (i == 0) {
                    index_page = i;
                    new_pos = temp;
                }
                break;
            }
            index_page = i;
            new_pos = temp;
        }
        current_pos_ = new_pos;
        engine_index_ = index_engine;
        page_index_ = index_page;
    }

    create_paint_other_engine(index_engine, count_engine);

    // 这样有问题 导致引擎序号错位
    // insert_cover_page();
}

void mobi_book::create_paint_other_engine(int32_t index, int32_t count)
{
    std::vector<int32_t> array;
    if (index + 1 < count) {
        array.push_back(index + 1);
    }
    if (index - 1 >= 0) {
        array.push_back(index - 1);
    }
    for (int i = index + 2; i < count; ++i) {
        array.push_back(i);
    }
    for (int i = index - 2; i >= 0; --i) {
        array.push_back(i);
    }

    for (const int32_t& item: array) {
        paint_engine* engine = new paint_engine(size_.fWidth, size_.fHeight, pages_[item]);

        {
            std::unique_lock<decltype(mutex_engine_)> lock(mutex_engine_);
            paint_engine_[item] = engine;
        }
    }
}

void mobi_book::update_current_pos()
{
    if (engine_index_ < 0 || engine_index_ >= paint_engine_.size()) return;
    paint_engine* engine = paint_engine_[engine_index_];
    if (!engine) return;
    int32_t page_index = page_index_;
    if (page_index < 0 || page_index >= engine->get_page_count()) {
        page_index = engine->get_page_count() - 1;
    }
    if (page_index < 0) return;
    paint_page* page = engine->get_page(page_index);
    if (!page) return;
    current_pos_ = page->get_start_pos();
}

void mobi_book::insert_cover_page()
{
    do
    {
        if (!cover_image_) break;
        paint_engine* cover = new paint_engine(size_.fWidth, size_.fHeight, cover_image_);
        if (!cover) break;
        if (cover->get_page_count() <= 0) {
            delete cover;
            break;
        }
        paint_engine_.insert(paint_engine_.begin(), cover);
    } while (false);
}
