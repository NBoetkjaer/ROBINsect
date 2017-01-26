#pragma once

#include "BaseNode.hpp"
extern Attribute rangeAttrib;

template <typename T>
class NumericNode : public GenericNode<T>
{
public:
	NumericNode(const std::string &nodeName, T val) : GenericNode<T>(nodeName, val) { }
	virtual ~NumericNode() {}

	virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue)
	{
		if (rangeAttrib.GetID() == attribID)
		{

		}
		else if (valueAttrib.GetID() == attribID)
		{

		}
		else
		{
			return GenericNode<T>::SetAttribute(attribID, pAttributeValue);
		}
		return true;
	}

	void SetRange(T min, T max)
	{
		// ToDo check min <= max
		minValue = min;
		maxValue = max;
	}

	virtual void Set(const T &newValue)
	{
		T val = std::min(maxValue, std::max(newValue, minValue));
		GenericNode<T>::Set(val);
	}
private:
	T minValue; // = std::numeric_limits<T>::lowest()
	T maxValue; // = std::numeric_limits<T>::max()
};

typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<float> FloatNode;