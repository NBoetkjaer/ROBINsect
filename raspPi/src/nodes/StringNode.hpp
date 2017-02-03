#pragma once

#include "AbstractValueNode.hpp" 

class StringNode : public AbstractValueNode<std::string>
{
public:
    StringNode(const std::string &nodeName, std::string val) : AbstractValueNode<std::string>(nodeName, val){}
    virtual ~StringNode(){}
    virtual void SetValue(const char* pValues)
    {
        value = pValues;
        SetAttributeChanged(valueAttrib.GetID());
    }
};

