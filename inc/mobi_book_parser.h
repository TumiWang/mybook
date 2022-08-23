#ifndef _EBOOK_CORE_MOBI_BOOK_PARSER_H_
#define _EBOOK_CORE_MOBI_BOOK_PARSER_H_

#include "mobi_attr.h"

// libxml2
#include "libxml/HTMLparser.h"

#include <stdint.h>
#include <vector>

class mobi_book;
class mobi_element;
class mobi_page;

class mobi_book_parser
{
public:
    static mobi_book* parser(const char* buf, int32_t len, const char* default_font);

protected:
    mobi_book_parser();
    ~mobi_book_parser();

protected:
    bool init(const char* default_font);
    bool parse(const std::string& content);
    mobi_book* release_book();

protected:
    bool ergodic_node_from_body(xmlNodePtr node);

protected:
    void create_last_page();

    void add_paragraph_if_need(int32_t pos);
    void add_paragraph(xmlNodePtr node);
    void add_image(xmlNodePtr node);
    void add_br(xmlNodePtr node);
    void add_text(xmlNodePtr node);
    void proc_font_node(xmlNodePtr node);
    void proc_b_node(xmlNodePtr node);
    void proc_a_node(xmlNodePtr node);
    void clear_element_stack();

    void check_align_attr(xmlNodePtr node);

    void init_element_attrs(mobi_element* element);

    void init_menu(xmlNodePtr guide);

private:
    mobi_book* book_;
    mobi_page* last_page_;

    std::vector<mobi_attr> attr_stack_;

    std::vector<mobi_element*> element_stack_;
};

#endif