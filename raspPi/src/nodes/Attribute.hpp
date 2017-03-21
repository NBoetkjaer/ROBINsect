#pragma once
#include <string>
#include <vector>

typedef int8_t attribID_t;
#define INVALID_ATTRIBUTE_ID (-1)

// All nodes types needs to register the different attributes that it intent to handle via the
// function 'SetAttribute'. The constructor 'Attribute(const char* name)' is used to register a 
// given attribute. If an attribute name is registred more than once, the same ID is used.

class Attribute
{
private:
    std::string name;
    attribID_t id;
    static std::vector<std::string>& GetAttributes();
public:
    Attribute(const char* name);
    virtual ~Attribute() {};

    inline explicit operator attribID_t() const { return id; } // explicit conversion to attribID_t
    inline attribID_t GetID() const {return id;}
    inline const std::string& GetName() const {return name;}
    static const std::string& GetAttributeName(attribID_t id);

    // Function used to lookup an attributes given its name.
    // Returns the unique ID for the given attribute name. 
    // If attribute is not found it returns INVALID_ATTRIBUTE_ID. 
    static attribID_t GetAttributeID(const char* attribName);
    static attribID_t GetNumAttributes();
};
