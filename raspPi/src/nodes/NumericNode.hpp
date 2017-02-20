#pragma once

#include "AbstractValueNode.hpp"
#include <sstream>
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
    NumericNode(const std::string &nodeName, T val) :
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
            return false; // ToDo
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

    void SetRange(const char* pValues)
    {
        T _minValue, _maxValue;
        int numSuccess = sscanf(pValues, pRangeFmt, &_minValue, &_maxValue);
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
        SetAttributeChanged(rangeAttrib.GetID()); // Mark the change.
        minValue = min;
        maxValue = max;
        Set(value); // This will ensure that 'value' is within the new range.

    }

    void GetRange(T& min, T& max) const { min = minValue; max = maxValue;}

    virtual void GetValue(std::string &strValue)
    {
        //ToDo ... check.
        static const int maxLength = 30;
        strValue.resize(maxLength+1);
        int len = snprintf(&strValue[0], maxLength, pFmt, value);
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
};

typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<int64_t> Int64Node;
typedef NumericNode<float> FloatNode;
typedef NumericNode<double> DoubleNode;