#ifndef _XML_UTIL_H_
#define _XML_UTIL_H_

#include "libxml/HTMLparser.h"

#include "include/core/SkTextBlob.h"

#include <functional>
#include <stdio.h>
#include <string>

class element_attrs;

bool StringToNumber(const std::string& text, int32_t& v);

bool GetContentFromXmlNode(xmlNodePtr node, std::string& content);

xmlNodePtr FinderNodeWith(xmlNodePtr node, const xmlChar* tag);

void WalkAttributes(xmlNodePtr node, std::function<void(const std::string& key, const std::string& v)> proc);

bool ErgodicSubXmlNode(xmlNodePtr node, std::function<bool(const xmlNodePtr cur, const std::string& content)> proc);
bool ErgodicXmlNode(xmlNodePtr node, std::function<bool(const xmlNodePtr cur, const std::string& content)> proc);

bool GetPropFromNode(xmlNodePtr node, bool ergodic, const xmlChar* prop, std::string& v);

bool GetIntPropFromNode(xmlNodePtr node, bool ergodic, const xmlChar* prop, int32_t& v);

bool IsTailNode(xmlNodePtr node);

sk_sp<SkTextBlob> CreateTextBlob(const std::string& node_content, const element_attrs& attrs, int32_t max_width, int cur_index, int& last_index);
sk_sp<SkTextBlob> CreateTextBlob(xmlNodePtr node, const element_attrs& attrs, int32_t max_width, int cur_index, int& last_index);

#endif