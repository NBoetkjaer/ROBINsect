#include "Attribute.hpp"

#include <algorithm>
#include <assert.h>
#include <limits>
using namespace std;

Attribute::Attribute(const char* pName):
    name(pName)
{
    id = GetAttributeID(pName);
    if (id == INVALID_ATTRIBUTE_ID)
    {
        id = (attribID_t)(GetAttributes().size());
        if (id >= std::numeric_limits<attribID_t>::max())
        {
            // ToDo - fix exception handling ...
            throw std::exception(); // (("Too many attributes registred.");
        }
        GetAttributes().push_back(pName);
    }
}

std::vector<std::string>& Attribute::GetAttributes()
{
    // Implement a singleton pattern 
    // Note that pRegistredAttributes will leak on process termination, but the OS will reclaim the memory.
    static std::vector<std::string> *pRegistredAttributes = new std::vector<std::string>();
    return *pRegistredAttributes;
}

const string& Attribute::GetAttributeName(attribID_t id)
{
    assert(id >= 0 && id < GetAttributes().size());
    return GetAttributes().at(id);
}

attribID_t Attribute::GetAttributeID(const char* attribName)
{
    auto it = std::find(GetAttributes().cbegin(), GetAttributes().cend(), attribName);
    if (it == GetAttributes().cend())
    {        
        return INVALID_ATTRIBUTE_ID;
    }
    return std::distance(GetAttributes().cbegin(), it);
}


attribID_t Attribute::GetNumAttributes()
{ 
    return (attribID_t)GetAttributes().size();
}