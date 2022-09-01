#include "ebook_core.h"

#include "mobi_book_parser.h"
#include "mobi_book.h"

book_core* CreateBookCore(const char* buf, int32_t len)
{
    mobi_book* mobi_book = mobi_book_parser::parser(buf, len);
    if (mobi_book) return mobi_book;
    return NULL;
}

void CloseBookCore(book_core* book)
{
    if (!book) return;
    if (book->is_type("mobi_book")) {
        mobi_book* mobi = (mobi_book*)book;
        delete mobi;
    }
}

std::string GetFontnameFromLanguage(const std::string& language)
{
    std::string font_name;
    if (language == "zh" || language == "zh-CN") {
        // 简体中文
        font_name = "PingFang SC";
    } else if (language == "zh-TW") {
        // 繁体中文
        font_name = "PingFang TC";
    } else if (language == "ja") {
        // 日文
        font_name = "Hiragino Kaku Gothic ProN";
    } else if (language == "ko") {
        // 韩文
        font_name = "Apple SD Gothic Neo";
    } else {
        auto pos = language.find("zh");
        if (pos != std::string::npos) {
            // 简体中文
            font_name = "PingFang SC";
        }
    }
    return font_name;
}
