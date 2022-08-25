#include "mobi_book_parser.h"

#include "conv_code.h"
#include "mobi_book.h"
#include "mobi_element_br.h"
#include "mobi_element_image.h"
#include "mobi_element_paragraph.h"
#include "mobi_element_text.h"
#include "mobi_element.h"
#include "mobi_page.h"

#include "expat.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "mobipocket.h"
#ifdef __cplusplus
}
#endif

#include <algorithm>
#include <string.h>

static std::string GetStringFromMobiBinary(const char* buf, int len, uint32_t text_encoding)
{
    std::string text;
    switch (text_encoding)
    {
    case MOBI_ENCODING_WINLATIN1:
        if (!cp1252_to_utf8(buf, len, text)) {
            text.clear();
        }
        break;
    case MOBI_ENCODING_UTF8:
        text = std::string(buf, len);
        break;
    default:
        break;
    }
    return text;
}

static std::string GetContentFromMobi(mobipocket_t& pocket)
{
    std::string content;
    for (int index = pocket.mobi_header.first_content_index; index <= pocket.mobi_header.last_content_index; ++index) {
        if (index >= pocket.mobi_header.first_image_index) {
            break;
        }
        if (index >= pocket.mobi_header.first_non_book_index) {
            break;
        }
        if (pocket.palmdoc_header.compression == 1) {
            // 无压缩
            pdb_record_t* p = pocket.pdb.records.record + index;
            content += GetStringFromMobiBinary(p->data, p->data_len, pocket.mobi_header.text_encoding);
        }
        else if (pocket.palmdoc_header.compression == 2) {
            // lz77压缩
            pdb_record_t* p = pocket.pdb.records.record + index;
            uint16_t len = lz77_decode_length(p->data, p->data_len);
            char* temp = (char*)malloc(len * sizeof(char));
            uint16_t ll = lz77_decode(temp, p->data, p->data_len);
            if (len < pocket.palmdoc_header.record_size) {
                content += GetStringFromMobiBinary(temp, len, pocket.mobi_header.text_encoding);
            }
            else {
                content += GetStringFromMobiBinary(temp, pocket.palmdoc_header.record_size, pocket.mobi_header.text_encoding);
            }
            free(temp);
        }
        else if (pocket.palmdoc_header.compression == 17480) {
            // 霍夫曼编码
            break;
        }
        else {
            break;
        }
    }
    return content;
}

// static
mobi_book* mobi_book_parser::parser(const char* buf, int32_t len, const char* default_font)
{
    mobi_book* book = NULL;
    mobi_book_parser* book_parser = NULL;
    do {
        book_parser = new mobi_book_parser();
        if (!book_parser) break;
        if (!book_parser->init(default_font)) break;
        mobipocket_t pocket;
        init_mobipocket(&pocket);
        MemoryFile memoryfile = { 0 };
        memoryfile.buff = (char*)buf;
        memoryfile.buff_size = len;
        memoryfile.current_pos = 0;
        std::string content;
        std::string title;
        if (!read_memory_mobipocket(&memoryfile, &pocket)) {
            content = GetContentFromMobi(pocket);
            title = GetStringFromMobiBinary(pocket.full_name, strlen(pocket.full_name), pocket.mobi_header.text_encoding);
            if (book_parser->book_) {
                for (int index = pocket.mobi_header.first_image_index; index <= pocket.mobi_header.last_content_index; ++index) {
                    pdb_record_t* p = pocket.pdb.records.record + index;
                    std::vector<char> buf_temp;
                    buf_temp.resize(p->data_len);
                    memcpy(&(buf_temp[0]), p->data, p->data_len);
                    book_parser->book_->images_[index + 1 - pocket.mobi_header.first_image_index] = buf_temp;
                }
            }
        }
        free_mobipocket(&pocket);
        if (content.empty()) break;
        if (!book_parser->parse(content)) break;
        book = book_parser->release_book();
        if (book) book->title_ = title;
    } while(false);

    if (book_parser) {
        delete book_parser;
        book_parser = NULL;
    }
    return book;
}

mobi_book_parser::mobi_book_parser()
    : book_(NULL),
      last_page_(NULL),
      xml_parser_(NULL)
{

}

mobi_book_parser::~mobi_book_parser()
{
    if (book_) delete book_;
}

bool mobi_book_parser::init(const char* default_font)
{
    bool result = false;
    do {
        book_ = new mobi_book();
        if (!book_) break;
        create_last_page();

        attr_stack_.emplace_back(mobi_attr::mobi_attr_type::font_size, 3);
        attr_stack_.emplace_back(mobi_attr::mobi_attr_type::h_align, mobi_attr::h_align_type::left);
        attr_stack_.emplace_back(mobi_attr::mobi_attr_type::v_align, mobi_attr::v_align_type::baseline);
        attr_stack_.emplace_back(mobi_attr::mobi_attr_type::color, 0xff000000);
        std::string font_name;
        if (default_font) {
            font_name = default_font;
        } else {
            font_name = "Times";
        }
        attr_stack_.emplace_back(mobi_attr::mobi_attr_type::font_name, font_name);

        result = true;
    } while(false);
    return result;
}

bool mobi_book_parser::parse(const std::string& content)
{
    bool result = false;
    do {
        if (xml_parser_) break;
        xml_parser_ = XML_ParserCreate(NULL);
        if (!xml_parser_) break;
        XML_SetUserData(xml_parser_, this);
        XML_SetElementHandler(xml_parser_, mobi_book_parser::OnElementStart, mobi_book_parser::OnElementEnd);
        XML_SetCharacterDataHandler(xml_parser_, mobi_book_parser::OnElementText);
        int content_len = strlen(content.c_str());
        parser_head_ = false;
        parser_body_ = true;
        parser_finish_ = false;
        if (XML_Parse(xml_parser_, content.c_str(), content_len, 1) == XML_STATUS_ERROR) {
            if (!parser_finish_) break;
        }
        XML_ParserReset(xml_parser_, NULL);
        XML_SetUserData(xml_parser_, this);
        XML_SetElementHandler(xml_parser_, mobi_book_parser::OnElementStart, mobi_book_parser::OnElementEnd);
        XML_SetCharacterDataHandler(xml_parser_, mobi_book_parser::OnElementText);
        parser_head_ = true;
        parser_body_ = false;
        parser_finish_ = false;
        if (XML_Parse(xml_parser_, content.c_str(), content_len, 1) == XML_STATUS_ERROR) {
            if (!parser_finish_) break;
        }
        result = true;
    } while(false);
    if (xml_parser_) {
        XML_ParserFree(xml_parser_);
        xml_parser_ = NULL;
    }
    return result;
}

mobi_book* mobi_book_parser::release_book()
{
    mobi_book* book = book_;
    book_ = NULL;
    return book;
}

// static
void XMLCALL mobi_book_parser::OnElementStart(void * data, const XML_Char * tag, const XML_Char **attr)
{
    mobi_book_parser* parser = (mobi_book_parser*)data;
    if (!parser) return;
    if (!parser->xml_parser_) return;

    if (!parser->in_head_ && !parser->in_body_) {
        if (!strcasecmp(tag, "head")) {
            if (parser->parser_head_) parser->in_head_ = true;
        } else if (!strcasecmp(tag, "body")) {
            if (parser->parser_body_) parser->in_body_ = true;
        }
    }

    if (parser->in_head_) {
        if (!strcasecmp(tag, "reference")) {
            int32_t start_pos = XML_GetCurrentByteIndex(parser->xml_parser_);
            int32_t end_pos = start_pos + XML_CurrentNodeCount(parser->xml_parser_);
            parser->proc_menu(tag, attr, start_pos, end_pos);
        }
    } else if (parser->in_body_) {
        int32_t start_pos = XML_GetCurrentByteIndex(parser->xml_parser_);
        int32_t end_pos = start_pos + XML_CurrentNodeCount(parser->xml_parser_);
        int bIsSingle = XML_CurrentNodeIsSigle(parser->xml_parser_);

        if (!bIsSingle) {
            parser->parser_pos_stack_.push_back(start_pos);
        }

        if (start_pos < 0) return;
        if (end_pos < start_pos) return;

        if (strcasecmp(tag, "p") == 0 ||
            strcasecmp(tag, "blockquote") == 0) {
            parser->clear_element_stack();
            parser->add_paragraph(tag, attr, start_pos, end_pos);
        } else if (strcasecmp(tag, "mbp:pagebreak") == 0) {
            parser->clear_element_stack();
            parser->create_last_page();
        } else if (strcasecmp(tag, "img") == 0) {
            parser->add_paragraph_if_need(start_pos);
            parser->add_image(tag, attr, start_pos, end_pos);
        } else if (strcasecmp(tag, "br") == 0) {
            parser->add_paragraph_if_need(start_pos);
            parser->clear_element_stack();
            parser->add_br(tag, attr, start_pos, end_pos);
        } else if (strcasecmp(tag, "font") == 0) {
            parser->proc_font_node(tag, attr, start_pos, end_pos);
        } else if (strcasecmp(tag, "b") == 0) {
            parser->proc_b_node(tag, attr, start_pos, end_pos);
        } else if (strcasecmp(tag, "a") == 0) {
            parser->proc_a_node(tag, attr, start_pos, end_pos);
        }
    }
}

// static
void XMLCALL mobi_book_parser::OnElementEnd(void * data, const XML_Char * tag)
{
    mobi_book_parser* parser = (mobi_book_parser*)data;
    if (!parser) return;
    if (!parser->xml_parser_) return;

    if (parser->in_head_) {
        if (!strcasecmp(tag, "head")) {
            parser->in_head_ = false;
        }
    } else if (parser->in_body_) {
        if (!parser->parser_pos_stack_.empty()) {
            int32_t pos =parser-> parser_pos_stack_.back();
            auto itor = std::remove_if(parser->attr_stack_.begin(), parser->attr_stack_.end(), [&](const mobi_attr& item)->bool { return item.get_pos() >= pos; });
            if (itor != parser->attr_stack_.end()) {
                parser->attr_stack_.erase(itor, parser->attr_stack_.end());
            }
            parser->parser_pos_stack_.pop_back();
        }
        if (!strcasecmp(tag, "body")) {
            parser->in_body_ = false;
        }
    } else {
        if (!strcasecmp(tag, "html")) {
            parser->parser_finish_ = true;
        }
    }
}

// static
void XMLCALL mobi_book_parser::OnElementText(void * data, const XML_Char * text, int len)
{
    mobi_book_parser* parser = (mobi_book_parser*)data;
    if (!parser) return;
    if (!parser->xml_parser_) return;

    if (!parser->in_body_ && !parser->in_head_) return;

    int32_t start_pos = XML_GetCurrentByteIndex(parser->xml_parser_);
    if (start_pos < 0) return;
    if (len < 0) return;

    parser->add_text(text, len, start_pos, start_pos + len);
}

void mobi_book_parser::create_last_page()
{
    do {
        if (!book_) break;
        mobi_page* page = new mobi_page();
        if (!page) break;
        book_->pages_.push_back(page);
        last_page_ = page;
    } while(false);
}

void mobi_book_parser::add_paragraph_if_need(int32_t pos)
{
    // if (element_stack_.empty()) {
    //     mobi_element_paragraph* p = new mobi_element_paragraph();
    //     p->start_pos_ = pos;
    //     last_page_->children_.push_back(p);
    // }
}

void mobi_book_parser::add_paragraph(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    mobi_element_paragraph* p = new mobi_element_paragraph();
    p->start_pos_ = start_pos;
    p->end_pos_ = end_pos;
    last_page_->children_.push_back(p);
    check_align_attr(tag, attr, start_pos, end_pos);
    init_element_attrs(p);
}

void mobi_book_parser::add_image(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    mobi_element_image* image = NULL;
    int index = 0;
    for (index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "recindex")) {
            int32_t value = 0;
            if (StringToNumber((char*)attr[index + 1], value)) {
                for (auto itor = book_->images_.begin(); itor != book_->images_.end(); ++itor) {
                    if (itor->first == value) {
                        image = new mobi_element_image(itor->second);
                        if (!(image->image_)) {
                            delete image;
                            image = NULL;
                        }
                        image->start_pos_ = start_pos;
                        image->end_pos_ = end_pos;
                        break;
                    }
                }
            }
            break;
        }
    }
    if (!image) return;
    last_page_->children_.push_back(image);

    check_align_attr(tag, attr, start_pos, end_pos);

    for (index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "width")) {
            int32_t value = 0;
            if (StringToNumber((char*)attr[index + 1], value)) {
                if (value > 0 && value <= 10000) {
                    image->width_ = value;
                }
            }
        } else if (!strcmp(attr[index], "height")) {
            int32_t value = 0;
            if (StringToNumber((char*)attr[index + 1], value)) {
                if (value > 0 && value <= 10000) {
                    image->height_ = value;
                }
            }
        }
    }

    init_element_attrs(image);
}

void mobi_book_parser::add_br(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    mobi_element_br* br = new mobi_element_br();
    br->start_pos_ = start_pos;
    br->end_pos_ = end_pos;
    last_page_->children_.push_back(br);
    init_element_attrs(br);
}

void mobi_book_parser::add_text(const XML_Char * text, int len, int32_t start_pos, int32_t end_pos)
{
    std::string content(text, len);
    mobi_element_text* text_node = new mobi_element_text((const char*)content.c_str());
    text_node->start_pos_ = start_pos;
    text_node->end_pos_ = end_pos;
    last_page_->children_.push_back(text_node);
    init_element_attrs(text_node);
}

void mobi_book_parser::proc_font_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    for (int index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "size")) {
            mobi_attr att(start_pos, "font_size", (const char*)(attr[index + 1]));
            if (att.get_type() != mobi_attr::mobi_attr_type::invilad) {
                attr_stack_.push_back(att);
            }
            break;
        }
    }
    check_align_attr(tag, attr, start_pos, end_pos);
}

void mobi_book_parser::proc_b_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    attr_stack_.emplace_back(start_pos, mobi_attr::mobi_attr_type::bold, 1);
}

void mobi_book_parser::proc_a_node(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    for (int index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "filepos")) {
            mobi_attr att(start_pos, "filepos", (const char*)attr[index + 1]);
            if (att.get_type() != mobi_attr::mobi_attr_type::invilad) {
                attr_stack_.push_back(att);
            }
            break;
        }
    }
}

void mobi_book_parser::clear_element_stack()
{
    // element_stack_.clear();
}

void mobi_book_parser::check_align_attr(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    for (int index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "align")) {
            mobi_attr att(start_pos, "align", (const char*)attr[index + 1]);
            if (att.get_type() != mobi_attr::mobi_attr_type::invilad) {
                attr_stack_.push_back(att);
            }
            break;
        }
    }
}

void mobi_book_parser::init_element_attrs(mobi_element* element)
{
    std::string fone_name;
    int32_t* address = NULL;
    int32_t* font_size = NULL;
    bool* bold = NULL;
    bool* italic = NULL;
    int32_t* color = NULL;
    mobi_attr::h_align_type* h_align = NULL;
    mobi_attr::v_align_type* v_align = NULL;

    for (int i = attr_stack_.size() - 1; i >= 0; --i) {
        const auto& item = attr_stack_[i];
        switch(item.get_type())
        {
            case mobi_attr::mobi_attr_type::font_size:
                if (!font_size) {
                    font_size = new int32_t;
                    *font_size = item.get_value();
                }
                break;
            case mobi_attr::mobi_attr_type::font_name:
                if (fone_name.empty()) {
                    fone_name = item.get_string();
                }
                break;
            case mobi_attr::mobi_attr_type::bold:
                if (!bold) {
                    bold = new bool;
                    if (item.get_value()) {
                        *bold = true;
                    } else {
                        *bold = false;
                    }
                }
                break;
            case mobi_attr::mobi_attr_type::italic:
                if (!italic) {
                    italic = new bool;
                    if (item.get_value()) {
                        *italic = true;
                    } else {
                        *italic = false;
                    }
                }
                break;
            case mobi_attr::mobi_attr_type::width:
                break;
            case mobi_attr::mobi_attr_type::height:
                break;
            case mobi_attr::mobi_attr_type::color:
                if (!color) {
                    color = new int32_t;
                    *color = item.get_value();
                }
                break;
            case mobi_attr::mobi_attr_type::h_align:
                if (!h_align) {
                    h_align = new mobi_attr::h_align_type;
                    switch(item.get_value())
                    {
                        case mobi_attr::h_align_type::right:
                            *h_align = mobi_attr::h_align_type::right;
                            break;
                        case mobi_attr::h_align_type::center:
                            *h_align = mobi_attr::h_align_type::center;
                            break;
                        default:
                            *h_align = mobi_attr::h_align_type::left;
                            break;
                    }
                }
                break;
            case mobi_attr::mobi_attr_type::v_align:
                if (!v_align) {
                    v_align = new mobi_attr::v_align_type;
                    switch(item.get_value())
                    {
                        case mobi_attr::v_align_type::top:
                            *v_align = mobi_attr::v_align_type::top;
                            break;
                        case mobi_attr::v_align_type::bottom:
                            *v_align = mobi_attr::v_align_type::bottom;
                            break;
                        default:
                            *v_align = mobi_attr::v_align_type::baseline;
                            break;
                    }
                }
                break;
            case mobi_attr::mobi_attr_type::link:
                if (!address) {
                    address = new int32_t;
                    *address = item.get_value();
                }
                break;
            case mobi_attr::mobi_attr_type::invilad:
            default:
                break;
        }
    }

    if (!font_size) {
        font_size = new int32_t;
        *font_size = 3;
    }
    if (!bold) {
        bold = new bool;
        *bold = false;
    }
    if (!italic) {
        italic = new bool;
        *italic = false;
    }
    if (!color) {
        color = new int32_t;
        *color = 0xff000000;
    }
    if (!h_align) {
        h_align = new mobi_attr::h_align_type;
        *h_align = mobi_attr::h_align_type::left;
    }
    if (!v_align) {
        v_align = new mobi_attr::v_align_type;
        *v_align = mobi_attr::v_align_type::baseline;
    }

    element->font_name_ = fone_name;
    element->address_ = address;
    element->font_size_ = font_size;
    element->bold_ = bold;
    element->italic_ = italic;
    element->color_ = color;
    element->h_align_ = h_align;
    element->v_align_ = v_align;
}

void mobi_book_parser::proc_menu(const XML_Char * tag, const XML_Char **attr, int32_t start_pos, int32_t end_pos)
{
    if (!book_) return;

    // assert(tag, "reference");
    std::string strType;
    std::string strTitle;
    int32_t addr = -1;
    for (int index = 0; attr[index]; index += 2) {
        if (!strcmp(attr[index], "type")) {
            strType = (char*)attr[index + 1];
        } else if (!strcmp(attr[index], "title")) {
            strTitle = (char*)attr[index + 1];
        } else if (!strcmp(attr[index], "filepos")) {
            StringToNumber((char*)attr[index + 1], addr);
        }
    }

    if (!book_->menu_) {
        book_->menu_ = new mobi_menu();
    }

    if (!book_->menu_) return;
    if (strType == "text") {
        do {
            if (strTitle.empty()) break;
            mobi_element_text* text_node = new mobi_element_text((const char*)strTitle.c_str());
            if (!text_node) break;
            text_node->start_pos_ = start_pos;
            text_node->end_pos_ = end_pos;
            book_->menu_->children_.emplace_back(true, text_node);
            init_element_attrs(text_node);
            text_node->address_ = new int32_t;
            *(text_node->address_) = addr;
        } while(false);
    } else if (strType == "toc") {
        for (const auto& page: book_->pages_) {
            if (page->children_.empty()) continue;
            if (page->get_start_pos() >= addr && page->get_end_pos() < addr) {
                for(auto& child: page->children_) {
                    if (addr < child->get_start_pos()) continue;
                    if (child->get_type() == mobi_element::br) continue;
                    if (child->get_type() == mobi_element::paragraph) continue;
                    book_->menu_->children_.emplace_back(false, child);
                }
                break;
            }
        }
    }
}