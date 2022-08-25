#ifndef _EBOOK_CORE_MOBI_BOOK_PARSER_H_
#define _EBOOK_CORE_MOBI_BOOK_PARSER_H_

#include "mobi_attr.h"

// libxml2
#include "libxml/HTMLparser.h"

#include "expat.h"

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
    static void XMLCALL OnElementStart(void * data, const XML_Char * tag, const XML_Char **attr);
    static void XMLCALL OnElementEnd(void * data, const XML_Char * tag);
    static void XMLCALL OnElementText(void * data, const XML_Char * text, int len);

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
    void add_paragraph(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void add_image(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void add_br(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void add_text(const XML_Char * text, int len, int32_t start_pos, int32_t end_pos);
    void proc_font_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void proc_b_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void proc_a_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);
    void clear_element_stack();

    void check_align_attr(xmlNodePtr node);
    void check_align_attr(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);

    void init_element_attrs(mobi_element* element);

    void init_menu(xmlNodePtr guide);
    void proc_menu(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos);

private:
    mobi_book* book_;
    mobi_page* last_page_;

    std::vector<mobi_attr> attr_stack_;

    std::vector<mobi_element*> element_stack_;

    XML_Parser xml_parser_;

    std::vector<int32_t> parser_pos_stack_;

    bool in_head_ = false;
    bool in_body_ = false;

    bool parser_head_ = false;
    bool parser_body_ = false;
    bool parser_finish_ = false;
};

#endif