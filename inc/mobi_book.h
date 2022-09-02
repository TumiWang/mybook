#ifndef _EBOOK_CORE_MOBI_BOOK_H_
#define _EBOOK_CORE_MOBI_BOOK_H_

#include <stdint.h>

#include "ebook_core.h"

#include "mobi_menu.h"
#include "mobi_page.h"
#include "paint_engine.h"

#include <mutex>
#include <vector>
#include <map>

class mobi_book: public book_core
{
public:
    mobi_book();
    virtual ~mobi_book();

// imple book_core
public:
    bool is_type(const char* type_name) const override;
    std::string get_title() const override;

    void set_size(SkSize size) override;
    void SetCurrentPos(int32_t pos) override;

    int32_t GetCurrentPos() const override;

    void PrivPage() override;
    void NextPage() override;
    void ProcClick(const SkPoint& point) override;
    void paint(const SkPoint& point, SkCanvas* canvas) override;

    book_menu_core* get_menu() const override;

protected:
    void cleanup_paint_engine();
    void create_paint_engine();
    void create_paint_other_engine(int32_t index, int32_t count);

    void update_current_pos();

protected:
    std::string title_;

    SkSize size_;
    int32_t current_pos_;
    
    std::vector<mobi_page*> pages_;

    mobi_menu* menu_ = NULL;

    std::mutex mutex_engine_;
    std::vector<paint_engine*> paint_engine_;
    int32_t engine_index_ = 0;
    int32_t page_index_ = 0;

    std::map<int32_t, std::vector<char>> images_;

private:
    friend class mobi_book_parser;
};

#endif