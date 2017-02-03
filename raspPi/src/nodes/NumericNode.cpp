#include "NumericNode.hpp"

Attribute valueAttrib("value");
Attribute rangeAttrib("range");
Attribute unitAttrib("unit");
Attribute prefixAttrib("prefix");

const char* Int32Node::pFmt = "%d";
const char* Int32Node::pRangeFmt = "[%d,%d]";

const char* Int64Node::pFmt = "%ld";
const char* Int64Node::pRangeFmt = "[%lld,%lld]";

const char* FloatNode::pFmt = "%g";
const char* FloatNode::pRangeFmt = "[%g,%g]";

const char* DoubleNode::pFmt = "%lg";
const char* DoubleNode::pRangeFmt = "[%lg,%lg]";