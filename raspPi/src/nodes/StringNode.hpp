#pragma once

#include "AbstractValueNode.hpp" 

class StringNode : public AbstractValueNode<std::string>
{
public:
    StringNode(const std::string &nodeName, std::string val = "") : AbstractValueNode<std::string>(nodeName, val){}
    ~StringNode() override {}
    
    void GetValue(std::string &strValue) const override
    {
        strValue = value;
    }

    void SetValue(const char* pValues) override
    {
        value = pValues;
        SetAttributeChanged(valueAttrib.GetID());
    }
};
