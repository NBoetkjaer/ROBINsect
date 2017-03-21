#include "NumericNode.hpp"

Attribute valueAttrib("value");
Attribute rangeAttrib("range");
Attribute unitAttrib("unit");
Attribute prefixAttrib("prefix");
template<>
const char* Int32Node::pFmt = "%d";
template<>
const char* Int32Node::pRangeFmt = "[%d,%d]";
template<>
const char* Int64Node::pFmt = "%lld";
template<>
const char* Int64Node::pRangeFmt = "[%lld,%lld]";
template<>
const char* FloatNode::pFmt = "%g";
template<>
const char* FloatNode::pRangeFmt = "[%g,%g]";
template<>
const char* DoubleNode::pFmt = "%lg";
template<>
const char* DoubleNode::pRangeFmt = "[%lg,%lg]";