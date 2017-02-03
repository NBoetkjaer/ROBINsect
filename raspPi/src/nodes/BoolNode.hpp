#pragma once

#include "AbstractValueNode.hpp" 
#if _MSC_VER
#define strcasecmp _stricmp
#endif

class BoolNode : public AbstractValueNode<bool>
{
public:
    BoolNode(const std::string &nodeName, bool val) : AbstractValueNode<bool>(nodeName, val){}
    virtual ~BoolNode(){}
    virtual void SetValue(const char* pValue)
    {
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
};
