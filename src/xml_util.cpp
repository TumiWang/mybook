#include "xml_util.h"

#include "include/core/SkFontMgr.h"

bool StringToNumber(const std::string& text, int32_t& v)
{
    bool result = true;
    int r = 0;
    bool negative = false;
    std::string temp = text;
    if (temp.size() > 1) {
        if (text[0] == '-') {
            negative = true;
            temp = temp.substr(1);
        }
    }
    for (const char& c: temp) {
        if (c >= '0' && c <= '9') {
            r = r * 10 + (c - '0');
            continue;
        }
        if (c == '.') break;
        result = false;
        break;
    }
    if (negative) r = r * (-1);
    if (result) v = r;
    return result;
}

bool GetContentFromXmlNode(xmlNodePtr node, std::string& content)
{
    bool result = false;
    xmlNodePtr child = NULL;
    do {
        if (!node) break;
        child = node->children;
        if (!child) break;
        if (child->next) break;
        if (strcmp((char*)child->name, "text")) break;
        xmlChar* text = xmlNodeGetContent(node);
        if (!text) break;
        content = (char*)text;
        // if (!child) break;
        // // 目前假设有且只有一个子
        // if (child->next) break;
        // if (strcmp((char*)child->name, "text")) break;
        // if (!(child->content)) break;
        // content = (char*)child->content;
        xmlFree(text);
        result = true;
    } while (false);
    return result;
}

xmlNodePtr FinderNodeWith(xmlNodePtr node, const xmlChar* tag)
{
    if (!node) return NULL;
    if (!xmlStrcasecmp(node->name, tag)) return node;
    xmlNodePtr cur = node->children;
    while(cur) {
        xmlNodePtr result = FinderNodeWith(cur, tag);
        if (result) return result;
        cur = cur->next;
    }
    return NULL;
}

void WalkAttributes(xmlNodePtr node, std::function<void(const std::string& key, const std::string& v)> proc)
{
    if (!node) return;
    // if (!(node->properties)) return;
    xmlAttrPtr attr = node->properties;
    while (attr) {
        std::string text;
        if (attr->name && attr->children && attr->children->name) {
            if (!xmlStrcasecmp(attr->children->name, BAD_CAST"text")) {
                if (attr->children->content) {
                    text = (char*)(attr->children->content);
                }
                proc((char*)(attr->name), text);
            }
        }
        attr = attr->next;
    }
}

bool ErgodicSubXmlNode(xmlNodePtr node, std::function<bool(const xmlNodePtr, const std::string&)> proc)
{
    std::string content;
    if (GetContentFromXmlNode(node, content)) {
        return proc(node, content);
    }
    if (!proc(node, content)) {
        return false;
    }
    xmlNodePtr cur = node->children;
    while (cur)
    {
        if (!ErgodicSubXmlNode(cur, proc))
        {
            return false;
        }
        cur = cur->next;
    }
    return true;
}

bool ErgodicXmlNode(xmlNodePtr node, std::function<bool(const xmlNodePtr, const std::string&)> proc)
{
    xmlNodePtr cur = node;
    while (cur) {
        if (!ErgodicSubXmlNode(cur, proc)) {
            return false;
        }
        while (cur) {
            if (cur->next) {
                cur = cur->next;
                break;
            }
            cur = cur->parent;
        }
    }
    return true;
}

bool GetPropFromNode(xmlNodePtr node, bool ergodic, const xmlChar* prop, std::string& v)
{
    xmlNodePtr cur = node;
    while (cur)
    {
        xmlChar* r = xmlGetProp(node, prop);
        if (r) {
            v = (char*)r;
            xmlFree(r);
            return true;
        }
        if (!ergodic) break;
        cur = cur->parent;
    }
    return false;
}

bool GetIntPropFromNode(xmlNodePtr node, bool ergodic, const xmlChar* prop, int32_t& v)
{
    bool result = false;
    std::string text;
    do
    {
        if (!GetPropFromNode(node, ergodic, prop, text)) break;
        if (!StringToNumber(text, v)) break;
        result = true;
    } while (false);
    
    return result;
}

bool IsTailNode(xmlNodePtr node)
{
    xmlNodePtr cur = node;
    while (cur)
    {
        if (!xmlStrcasecmp(cur->name, BAD_CAST"html")) return true;
        if (!xmlStrcasecmp(cur->name, BAD_CAST"body")) return true;
        if (!xmlStrcasecmp(cur->name, BAD_CAST"p")) return true;
        if (!xmlStrcasecmp(cur->name, BAD_CAST"blockquote")) return true;
        if (cur->next) break;
        cur = cur->parent;
    }
    
    return false;
}
