#pragma once
#include <string>
#include <vector>

typedef char attribID_t;
#define INVALID_ATTRIBUTE_ID (-1)

class Attribute
{
private:
    static std::vector<std::string> registredAttributes;
    std::string name;
    attribID_t id;
public:
    Attribute(const char* name);
    inline attribID_t GetID() const {return id;}
    inline const std::string& GetName() const {return name;}
    static const std::string& GetAttributeName(attribID_t id);

    // Function used to lookup an attributes given its name.
    // Returns the unique ID for the given attribute name. 
    // If attribute is not found it returns INVALID_ATTRIBUTE_ID. 
    static attribID_t GetAttributeID(const char* attribName);
    inline static size_t GetNumAttributes(){ return registredAttributes.size(); };
};


// All nodes types needs to register the different attributes that it intent to handle via the
// function 'SetAttribute'. The function 'RegisterAttribute' returns a unique number for each registered
// attribute. If an attribute is registred more than once, the same ID is returned.
// Ideally only the first instance of a given node type needs to do the registration.
//attribID_t RegisterAttribute(const char* attribName);
//attribID_t FindAttributeID(const char* attribName);

//attribID_t BaseNode::FindAttributeID(const char* attribName)
//{
//    auto it = std::find(registredAttributes.begin(), registredAttributes.end(), attribName);
//    if(it == registredAttributes.end())
//    {
//        return INVALID_ATTRIBUTE_ID;
//    }
//    return std::distance(registredAttributes.begin(), it);
//}
//
//attribID_t BaseNode::RegisterAttribute(const char* attribName)
//{
//    attribID_t id = FindAttributeID(attribName);
//    if(id == INVALID_ATTRIBUTE_ID)
//    {
//        registredAttributes.push_back(attribName);
//        return registredAttributes.size() - 1;
//    }
//    return id;
//}