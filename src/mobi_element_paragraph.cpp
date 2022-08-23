#include "mobi_element_paragraph.h"

mobi_element_paragraph::mobi_element_paragraph()
{

}

mobi_element_paragraph::~mobi_element_paragraph()
{

}

mobi_element::mobi_element_type mobi_element_paragraph::get_type() const
{
    return mobi_element::paragraph;
}
