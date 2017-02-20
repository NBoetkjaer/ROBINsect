#pragma once

#include "BaseNode.hpp"

extern Attribute valueAttrib;

template <typename T>
class AbstractValueNode : public BaseNode
{
public:
    AbstractValueNode(const std::string &nodeName, T val) : BaseNode(nodeName), value(val) { }
    virtual ~AbstractValueNode(){}
    virtual bool GetAttribute(attribID_t attribID, std::string &strAttributeValue)
    {
        if (valueAttrib.GetID() == attribID)
        {
            GetValue(strAttributeValue);
            return true;
        }
        else
        {
            return BaseNode::GetAttribute(attribID, strAttributeValue);
        }
        return false; // Attribute is unhandled.
    }

    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue)
    {
        if (valueAttrib.GetID() == attribID)
        {
            SetValue(pAttributeValue);
            return true;
        }
        else
        {
            return BaseNode::SetAttribute(attribID, pAttributeValue);
        }
        return false; // Attribute is unhandled.
    }

    virtual void GetValue(std::string &strValue) = 0;
    virtual void SetValue(const char* pValues) = 0;

    virtual void Set(const T &newValue)
    {
        if (newValue == value) return;
        value = newValue;
        SetAttributeChanged(valueAttrib.GetID());
    }
    T Get() const { return value; }
protected:
    T value;
};