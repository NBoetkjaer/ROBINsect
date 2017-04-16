#pragma once

#ifdef _MSC_VER
    #if _MSC_VER <= 1800
        #define snprintf _snprintf
    #endif
#endif

#include <sstream>
#include "AbstractValueNode.hpp"

extern Attribute rangeAttrib;
extern Attribute unitAttrib;
extern Attribute prefixAttrib;

template <typename T>
class NumericNode : public AbstractValueNode<T>
{   
    static const char* pFmt;
    static const char* pRangeFmt;
public:
    // Constructor
    NumericNode(const std::string &nodeName, T val, T minimimValue, T maximumValue):
        AbstractValueNode<T>(nodeName, val)
    {
        SetRange(minimimValue, maximumValue);
    }
    
    NumericNode(const std::string &nodeName, T val = T(0)):
        AbstractValueNode<T>(nodeName, val),
        minValue(std::numeric_limits<T>::lowest()),
        maxValue(std::numeric_limits<T>::max()) 
    {}
    // Destructor
    virtual ~NumericNode() {}

    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue)
    {
        if (rangeAttrib.GetID() == attribID)
        {
            SetRange(pAttributeValue);
            return true;
        }
        else if (unitAttrib.GetID() == attribID)
        {
            unit = pAttributeValue;
            BaseNode::SetAttributeChanged(attribID);
            return true;
        }
        else if (prefixAttrib.GetID() == attribID)
        {
            return false; // ToDo 
        }
        else
        {
            return AbstractValueNode<T>::SetAttribute(attribID, pAttributeValue);
        }
        return false; // Attribute is unhandled.
    }

    virtual bool GetAttribute(attribID_t attribID, std::string &strAttributeValue) const
    {
        if (rangeAttrib.GetID() == attribID)
        {
            GetRange(strAttributeValue);
            return true;
        }
        else if (unitAttrib.GetID() == attribID)
        {
            strAttributeValue = unit;
            return true;
        }
        else if (prefixAttrib.GetID() == attribID)
        {
            return false; // ToDo 
        }
        else
        {
            return AbstractValueNode<T>::GetAttribute(attribID, strAttributeValue);
        }
        return false; // Attribute is unhandled.
    }

    void GetRange(std::string &strRange) const 
    {
        static const int maxLength = 60;
        strRange.resize(maxLength + 1);
        int len = snprintf(&strRange[0], maxLength, pRangeFmt, minValue, maxValue);
        if (len < 0) strRange.clear();
        if (len > maxLength) len = maxLength;
        strRange.resize(len);
    }

    void SetRange(const char* pValues)
    {
        T _minValue, _maxValue;
        const char *pChar = pValues;
        // Skip leading white spaces.
        while (*pChar != 0 && isspace((unsigned char)*pChar)) { ++pChar; }
        int numSuccess = sscanf(pChar, pRangeFmt, &_minValue, &_maxValue);
        if (numSuccess == 2)
        {
            SetRange(_minValue, _maxValue);
        }
        else
        {
            // ToDo: Report error.
        }
    }

    void SetRange(T min, T max)
    {
        if (min > max)
        {
            // ToDo: Report error?
            return;
        }
        // Did anything change?
        if (min == minValue && max == maxValue)
        {
            return;
        }
        BaseNode::SetAttributeChanged(rangeAttrib.GetID()); // Mark the change.
        minValue = min;
        maxValue = max;
        Set(AbstractValueNode<T>::value); // This will ensure that 'value' is within the new range.

    }

    void GetRange(T& min, T& max) const { min = minValue; max = maxValue;}

    virtual void GetValue(std::string &strValue) const 
    {
        //ToDo ... check.
        static const int maxLength = 30;
        strValue.resize(maxLength+1);
        int len = snprintf(&strValue[0], maxLength, pFmt, AbstractValueNode<T>::value);
        if (len < 0) strValue.clear();
        if (len > maxLength) len = maxLength;
        strValue.resize(len);
    }

    virtual void SetValue(const char* pValues) override
    {
        T _value;
        int numSuccess = sscanf(pValues, pFmt, &_value);
        if (numSuccess == 1)
        {
            Set(_value);
        }
        else
        {
            // ToDo: Report error (parsing string).
        }
    }

    virtual void Set(const T &newValue) override
    {
        // ToDo: Report error if outside range?
        T val = std::min(maxValue, std::max(newValue, minValue));
        AbstractValueNode<T>::Set(val);
    }
private:
    T minValue;
    T maxValue;

    std::string unit;
    //std::string prefix;

};

typedef NumericNode<uint16_t> UInt16Node;
typedef NumericNode<uint32_t> UInt32Node;

typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<int64_t> Int64Node;

typedef NumericNode<float> FloatNode;
typedef NumericNode<double> DoubleNode;