#include "mobi_book_parser.h"

#include "conv_code.h"
#include "mobi_book.h"
#include "mobi_element_br.h"
#include "mobi_element_image.h"
#include "mobi_element_paragraph.h"
#include "mobi_element_text.h"
#include "mobi_element.h"
#include "mobi_page.h"
#include "xml_util.h"

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
      last_page_(NULL)
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
    xmlDocPtr doc = NULL;
    do {
        doc = htmlParseDoc((const xmlChar *)content.c_str(), "UTF8");
        if (!doc) break;
        xmlNodePtr root = xmlDocGetRootElement(doc);
        if (!root) break;
        xmlNodePtr body = FinderNodeWith(root, BAD_CAST"body");
        if (!body) break;
        if (!ergodic_node_from_body(body)) break;
        xmlNodePtr head = FinderNodeWith(root, BAD_CAST"head");
        if (head) {
            xmlNodePtr guide = head->children;
            while(guide) {
                if (!xmlStrcasecmp(guide->name, BAD_CAST"guide")) {
                    init_menu(guide);
                    break;
                }
                guide = guide->next;
            }
        }
        result = true;
    } while(false);
    if (doc) {
        xmlFreeDoc(doc);
        doc = NULL;
    }
    return result;
}

mobi_book* mobi_book_parser::release_book()
{
    mobi_book* book = book_;
    book_ = NULL;
    return book;
}

bool mobi_book_parser::ergodic_node_from_body(xmlNodePtr node)
{
    // proc
    if (xmlStrcasecmp(node->name, BAD_CAST"p") == 0 ||
        xmlStrcasecmp(node->name, BAD_CAST"blockquote") == 0) {
        clear_element_stack();
        add_paragraph(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"text") == 0) {
        add_paragraph_if_need(node->pos);
        add_text(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"pagebreak") == 0) {
        clear_element_stack();
        create_last_page();
    } else if (xmlStrcasecmp(node->name, BAD_CAST"img") == 0) {
        add_paragraph_if_need(node->pos);
        add_image(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"br") == 0) {
        add_paragraph_if_need(node->pos);
        clear_element_stack();
        add_br(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"font") == 0) {
        proc_font_node(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"b") == 0) {
        proc_b_node(node);
    } else if (xmlStrcasecmp(node->name, BAD_CAST"a") == 0) {
        proc_a_node(node);
    }

    xmlNodePtr cur = node->children;
    while (cur)
    {
        if (!ergodic_node_from_body(cur))
        {
            return false;
        }
        cur = cur->next;
    }

    auto itor = std::remove_if(attr_stack_.begin(), attr_stack_.end(), [&node](const mobi_attr& item)->bool { return item.get_pos() >= node->pos; });
    if (itor != attr_stack_.end()) {
         attr_stack_.erase(itor, attr_stack_.end());
    }
    return true;
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

void mobi_book_parser::add_paragraph(xmlNodePtr node)
{
    mobi_element_paragraph* p = new mobi_element_paragraph();
    p->start_pos_ = node->pos;
    last_page_->children_.push_back(p);
    check_align_attr(node);
    init_element_attrs(p);
}

void mobi_book_parser::add_image(xmlNodePtr node)
{
    mobi_element_image* image = NULL;
    xmlChar* r = xmlGetProp(node, BAD_CAST"recindex");
    if (r) {
        int32_t value = 0;
        if (StringToNumber((char*)r, value)) {
            for (auto itor = book_->images_.begin(); itor != book_->images_.end(); ++itor) {
                if (itor->first == value) {
                    image = new mobi_element_image(itor->second);
                    if (!(image->image_)) {
                        delete image;
                        image = NULL;
                    }
                    image->start_pos_ = node->pos;
                    break;
                }
            }
        }
        xmlFree(r);
    }
    if (!image) return;
    last_page_->children_.push_back(image);

    check_align_attr(node);

    r = xmlGetProp(node, BAD_CAST"width");
    if (r) {
        int32_t value = 0;
        if (StringToNumber((char*)r, value)) {
            if (value > 0 && value <= 10000) {
                image->width_ = value;
            }
        }
        xmlFree(r);
    }
    r = xmlGetProp(node, BAD_CAST"height");
    if (r) {
        int32_t value = 0;
        if (StringToNumber((char*)r, value)) {
            if (value > 0 && value <= 10000) {
                image->height_ = value;
            }
        }
        xmlFree(r);
    }
    init_element_attrs(image);
}

void mobi_book_parser::add_br(xmlNodePtr node)
{
    mobi_element_br* br = new mobi_element_br();
    br->start_pos_ = node->pos;
    last_page_->children_.push_back(br);
    init_element_attrs(br);
}

void mobi_book_parser::add_text(xmlNodePtr node)
{
    xmlChar* text = xmlNodeGetContent(node);
    if (text) {
        mobi_element_text* text_node = new mobi_element_text((const char*)text);
        text_node->start_pos_ = node->pos;
        last_page_->children_.push_back(text_node);
        check_align_attr(node);
        init_element_attrs(text_node);
        xmlFree(text);
    }
}

void mobi_book_parser::proc_font_node(xmlNodePtr node)
{
    xmlChar* r = xmlGetProp(node, BAD_CAST"size");
    if (r) {
        mobi_attr attr(node->pos, "font_size", (char*)r);
        if (attr.get_type() != mobi_attr::mobi_attr_type::invilad) {
            attr_stack_.push_back(attr);
        }
        xmlFree(r);
    }
    check_align_attr(node);
}

void mobi_book_parser::proc_b_node(xmlNodePtr node)
{
    attr_stack_.emplace_back(node->pos, mobi_attr::mobi_attr_type::bold, 1);
}

void mobi_book_parser::proc_a_node(xmlNodePtr node)
{
    xmlChar* r = xmlGetProp(node, BAD_CAST"filepos");
    if (r) {
        mobi_attr attr(node->pos, "filepos", (char*)r);
        if (attr.get_type() != mobi_attr::mobi_attr_type::invilad) {
            attr_stack_.push_back(attr);
        }
        xmlFree(r);
    }
}

void mobi_book_parser::clear_element_stack()
{
    // element_stack_.clear();
}

void mobi_book_parser::check_align_attr(xmlNodePtr node)
{
    xmlChar* r = xmlGetProp(node, BAD_CAST"align");
    if (r) {
        mobi_attr attr(node->pos, "align", (char*)r);
        if (attr.get_type() != mobi_attr::mobi_attr_type::invilad) {
            attr_stack_.push_back(attr);
        }
        xmlFree(r);
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

void mobi_book_parser::init_menu(xmlNodePtr guide)
{
    if (!book_) return;
    if (book_->menu_) return;
    mobi_menu* menu = new mobi_menu();
    if (!menu) return;

    for (xmlNodePtr reference = guide->children; reference != NULL; reference = reference->next) {
        if (xmlStrcasecmp(reference->name, BAD_CAST"reference")) {
            continue;
        }
        std::string temp;
        xmlChar* r = xmlGetProp(reference, BAD_CAST"type");
        if (r) {
            temp = (char*)r;
            xmlFree(r);
        } else {
            continue;
        }
        if (temp != "text") continue;
        mobi_element_text* text_node = NULL;
        r = xmlGetProp(reference, BAD_CAST"title");
        if (r) {
            text_node = new mobi_element_text((const char*)r);
            text_node->start_pos_ = reference->pos;
            menu->children_.emplace_back(true, text_node);
            xmlFree(r);
        } else {
            continue;
        }
        int32_t* addr = NULL;
        r = xmlGetProp(reference, BAD_CAST"filepos");
        if (r) {
            addr = new int32_t;
            if (!StringToNumber((char*)r, *addr)) {
                *addr = -1;
            }
            xmlFree(r);
        }
        init_element_attrs(text_node);
        if (addr) {
            if (text_node->address_) delete text_node->address_;
            text_node->address_ = addr;
        }
    }

    for (xmlNodePtr reference = guide->children; reference != NULL; reference = reference->next) {
        if (xmlStrcasecmp(reference->name, BAD_CAST"reference")) {
            continue;
        }
        std::string temp;
        xmlChar* r = xmlGetProp(reference, BAD_CAST"type");
        if (r) {
            temp = (char*)r;
            xmlFree(r);
        } else {
            continue;
        }
        if (temp != "toc") continue;
        // r = xmlGetProp(reference, BAD_CAST"title");
        // if (r) {
        //     mobi_element_text* text_node = new mobi_element_text((const char*)r);
        //     text_node->start_pos_ = reference->pos;
        //     init_element_attrs(text_node);
        //     menu->children_.emplace_back(true, text_node);
        //     xmlFree(r);
        // }
        r = xmlGetProp(reference, BAD_CAST"filepos");
        if (r) {
            int32_t addr = -1;
            if (StringToNumber((char*)r, addr)) {
                for (const auto& item: book_->pages_) {
                    auto size = item->children_.size();
                    if (size <= 0) continue;
                    if (item->children_[0]->get_start_pos() > addr) continue;
                    if (item->children_[size - 1]->get_start_pos() < addr) continue;
                    for(auto& child: item->children_) {
                        if (child->get_start_pos() < addr) continue;
                        menu->children_.emplace_back(false, child);
                    }
                    break;
                }
            }
            xmlFree(r);
        }
    }
}
