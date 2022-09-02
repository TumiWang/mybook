#ifndef _EBOOK_CORE_BOOK_CORE_H_
#define _EBOOK_CORE_BOOK_CORE_H_

#include "ebook_menu_core.h"

// Skia
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSize.h"

#include <string>

class book_core
{
public:
    book_core() = default;
    ~book_core() = default;

public:
    virtual bool is_type(const char* type_name) const = 0;
    virtual std::string get_title() const = 0;

    virtual void set_size(SkSize size) = 0;
    virtual void SetCurrentPos(int32_t pos) = 0;

    virtual int32_t GetCurrentPos() const = 0;

    virtual void PrivPage() = 0;
    virtual void NextPage() = 0;
    virtual void ProcClick(const SkPoint& point) = 0;
    virtual void paint(const SkPoint& point, SkCanvas* canvas) = 0;

    virtual book_menu_core* get_menu() const = 0;
};

book_core* CreateBookCore(const char* buf, int32_t len);
void CloseBookCore(book_core* book);

#endif