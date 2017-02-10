#pragma once

#include "AbstractValueNode.hpp" 

class BoolNode : public AbstractValueNode<bool>
{
public:
    BoolNode(const std::string &nodeName, bool val) : AbstractValueNode<bool>(nodeName, val){}
    virtual ~BoolNode(){}
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue)
    {
        if (optionsAttrib.GetID() == attribID)
        {
            SetOptions(pAttributeValue, options);
            return true;
        }
        else
        {
            return AbstractValueNode::SetAttribute(attribID, pAttributeValue);
        }
        return false; // Attribute is unhandled.
    }

    virtual void SetValue(const char* pValue)
    {
        if (options.size() == 2)
        {
            int value = -1;
            SetOptionValue(pValue, options, value);
            if (value >= 0)
            {
                Set(value!=0);
            }
        }
        else
        {
            // Default
            if (strcasecmp("true", pValue) == 0)
            {
                Set(true);
            }
            else if (strcasecmp("false", pValue) == 0)
            {
                Set(false);
            }
            else
            {
                // ToDo: Report error (parsing string).
            }
        }
    }
private:
    std::vector<std::string> options;
};
