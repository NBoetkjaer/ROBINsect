#include "Attribute.hpp"

#include <algorithm>
using namespace std;


std::vector<std::string> Attribute::registredAttributes;

Attribute::Attribute(const char* pName):
    name(pName)
{
    id = GetAttributeID(pName);
    if(id == INVALID_ATTRIBUTE_ID)
    {
        id = (attribID_t)(registredAttributes.size());
		if (id >= sizeof(size_t) * 8)
		{
			throw std::exception("Too many attributes registred.");
		}
		registredAttributes.push_back(pName);
    }
}

const string& Attribute::GetAttributeName(attribID_t id)
{
    //if(id < 0 && id >= registredAttributes.size())
    return registredAttributes.at(id);
}

attribID_t Attribute::GetAttributeID(const char* attribName)
{
    auto it = std::find(registredAttributes.begin(), registredAttributes.end(), attribName);
    if(it == registredAttributes.end())
    {
        return INVALID_ATTRIBUTE_ID;
    }
    return std::distance(registredAttributes.begin(), it);
}

