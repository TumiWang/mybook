#include "mobi_page.h"

mobi_page::mobi_page()
{

}

mobi_page::~mobi_page()
{
    for (auto& item: children_)
    {
        delete item;
    }
    children_.clear();
}

int32_t mobi_page::get_start_pos() const
{
    if (children_.size() == 0) return -1;
    return children_.front()->get_start_pos();
}

int32_t mobi_page::get_end_pos() const
{
    if (children_.size() == 0) return -1;
    return children_.back()->get_end_pos();
}

bool mobi_page::traversal(const std::function<bool(mobi_element*, int32_t, int32_t)>& proc) const
{
    int32_t count = children_.size();
    for (int32_t index = 0; index < count; ++index) {
        if (!proc(children_[index], index, count)) return false;
    }
    return true;
}
