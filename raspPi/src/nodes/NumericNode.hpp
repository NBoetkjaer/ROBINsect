#pragma once

#include "BaseNode.hpp" 
#include <sstream>
extern Attribute rangeAttrib;

template <typename T>
class NumericNode : public GenericValueNode<T>
{   
    static const char* pFmt;
    static const char* pRangeFmt;
public:
    // Constructor
    NumericNode(const std::string &nodeName, T val) :
        GenericValueNode<T>(nodeName, val),
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
        }
        else
        {
            return GenericValueNode<T>::SetAttribute(attribID, pAttributeValue);
        }
        return true;
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

    virtual void SetValue(const char* pValues)
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

    virtual void Set(const T &newValue)
    {
        // ToDo: Report error if outside range?
        T val = std::min(maxValue, std::max(newValue, minValue));
        GenericValueNode<T>::Set(val);
    }
private:
    T minValue;
    T maxValue;
};

class BoolNode : public GenericValueNode<bool>
{
public:
    BoolNode(const std::string &nodeName, bool val) : GenericValueNode<bool>(nodeName, val){}
    virtual ~BoolNode(){}
    virtual void SetValue(const char* pValues)
    {
        bool _value;
        bool bSuccess = false;
        // ToDo: Parse string 
        if (bSuccess)
        {
            Set(_value);
        }
        else
        {
            // ToDo: Report error (parsing string).
        }
    }
};

typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<float> FloatNode;