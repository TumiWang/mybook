#include "ebook_core.h"

#include "mobi_book_parser.h"
#include "mobi_book.h"

book_core* CreateBookCore(const char* buf, int32_t len, const char* default_font)
{
    mobi_book* mobi_book = mobi_book_parser::parser(buf, len, default_font);
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
