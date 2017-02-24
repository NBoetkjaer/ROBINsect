#pragma once
#include <array>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <memory>
#include <iostream>
#include <typeinfo>
#include "Attribute.hpp"

#include "../util/BitmaskEnumClass.h"

#if _MSC_VER
#define strcasecmp _stricmp
#endif
//enum class NodeType { nodeNode, boolNode, intNode, floatNode, doubleNode, stringNode, enumNode };

enum class FlagType :uint32_t {hide, readonly, logging, persist, query, numflags, invalidFlag = numflags};
ENABLE_BITMASK_OPERATORS(FlagType) // Supply overloaded bitwise operators for FlagType.

// Attributes: Value, Type, Range, flags, Enums, DisplayName, DisplayFormat, unit, prefix, info, ... 
// Common attributes.
extern Attribute flagsAttrib;
extern Attribute typeAttrib;
extern Attribute infoAttrib;
extern Attribute optionsAttrib;

class NodeObserver
{
public:
    virtual void Notify()=0;
};

class BaseNode
{
private:
    // Common attributes.
    //static Attributes displayName("displayName");
    static std::array< std::tuple<const std::string, FlagType>, (size_t)FlagType::numflags> flagNames;
    bool isChanged;
    size_t changes; // Variable to hold the applied changes to this node. Each attribID has a bit in changes.
    size_t touchedAttributes; // Variable used to collect all used (active) attributes.
    //std::map<size_t, string> attributes; key<attribID,string>
    std::vector<NodeObserver*> subscribers;

protected:
    std::string name;
    BaseNode* pParent;
    std::vector<std::unique_ptr<BaseNode>> children;
    void Notify();
public:
    BaseNode(const std::string &nodeName, BaseNode* pParentNode = nullptr);
    virtual ~BaseNode();

    const std::string&  GetName() const { return name; }
    BaseNode* GetParent() const { return pParent; }

    // -------- Atributes --------
    // Get the string representation of a given attribute.
    virtual bool GetAttribute(attribID_t attribID, std::string &strAttributeValue) const;
    // Sets the given attribute, based on the string argument.
    // Inherited nodes should call and return a base implementation if it does not handle the attributeID.
    // Returns true is the attribute is handled. 
    bool SetAttribute(attribID_t attribID, const std::string &strAttributeValue)
    {
        return SetAttribute(attribID, strAttributeValue.c_str());
    }
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue);

    // Mark a given attribute as changed.
    inline void SetAttributeChanged(attribID_t attribID)
    { 
        if (changes == 0)
        {   // Notify the node tree that a change has been applied.
            SetNodeChanged();
        }
        changes |= (size_t)1 << attribID;
        touchedAttributes |= changes;
    }

    inline bool IsAttributeChanged(attribID_t attribID) const { return (changes & ((size_t)1 << attribID))!=0;}
    inline bool IsAttributeUsed(attribID_t attribID) const { return (touchedAttributes  & ((size_t)1 << attribID)) != 0; }

private:
    FlagType nodeFlag;
    std::string info;

    // Flag attribute methods.
public: void SetFlags(const char* pValues);
private: void GetFlags(std::string &strFlags) const;
public: void SetFlag(FlagType flag, bool value);
    bool GetFlag(FlagType flag) const;
    // Info attributes methods.
    const std::string &GetInfo() const {return info;}
    void SetInfo(const char* pValue);

    // Add/remove observers to this node.
    void Subscribe(NodeObserver* pObserver);
    void UnSubscribe(NodeObserver* pObserver);

private:
    BaseNode* FindNodeInternal(const char * pNodePath);
public:
    BaseNode* FindNode(const std::string& nodePath);
    BaseNode* FindNode(const char * pNodePath);

    template<typename TNode, typename ...Args>
    BaseNode* AddChild(Args&&... params)
    {
        std::unique_ptr<BaseNode> newNode(new TNode(std::forward<Args>(params)...));
        newNode->pParent = this;
        children.push_back(std::move(newNode));
        return children.back().get();
    }

    void SetNodeChanged();
    void Print(int indentLevel = 0) const;

    static void GetOptions(std::string& strOptions, const std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptions(const char* pValues, std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptionValue(const char* pValue, const std::vector<std::string> & options, int &value);
};

