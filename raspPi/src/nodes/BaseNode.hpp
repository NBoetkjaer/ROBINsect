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

    size_t recentChanges;    // Indicate if this node or any of its child have changed. LSB indicate newest change, remaing bits act as a counter.
    size_t attributeChanges; // Variable to hold the applied changes to this node. Each attribID has a bit in changes.
    size_t touchedAttributes; // Variable used to collect all used (active) attributes.
    //std::map<size_t, string> attributes; key<attribID,string>
    std::vector<NodeObserver*> subscribers;

protected:
    std::string name;
    BaseNode* pParent;
    std::vector<std::unique_ptr<BaseNode>> children;
public:
    BaseNode(const std::string &nodeName, BaseNode* pParentNode = nullptr);
    virtual ~BaseNode();

    const std::string&  GetName() const { return name; };
    BaseNode* GetParent() const { return pParent; };
    BaseNode* GetRoot() const;

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
        if (attributeChanges == 0)
        {   // Notify the node tree that a change has been applied.
            SetNodeChanged();
        }
        attributeChanges |= (size_t)1 << attribID;
        touchedAttributes |= attributeChanges;
    }
    inline bool IsAttributeChanged(attribID_t attribID) const { return (attributeChanges & ((size_t)1 << attribID))!=0;}
    inline bool IsAttributeUsed(attribID_t attribID) const { return (touchedAttributes  & ((size_t)1 << attribID)) != 0; }
    inline bool AnyChanges() { return recentChanges != 0; };
    inline bool AnyRecentChanges() { return recentChanges & 1; };
    inline int RecentChangeCount(){ return (recentChanges >> 2) + (recentChanges & 1); };


    void PushChangeHistory();
    void ClearAllChanges();
private:
    FlagType nodeFlag;
    std::string info;

    // Flag attribute methods.
public:
    void SetFlags(const char* pValues);
private:
    void GetFlags(std::string &strFlags) const;
public:
    void SetFlag(FlagType flag, bool value);
    bool GetFlag(FlagType flag) const;
    // Info attributes methods.
    const std::string &GetInfo() const {return info;}
    void SetInfo(const char* pValue);

    // Add/remove observers to this node.
    void Subscribe(NodeObserver* pObserver);
    void UnSubscribe(NodeObserver* pObserver);
    void Notify(bool recursive); // Notify all subscribers

private:
    BaseNode* FindNodeInternal(const char * pNodePath);
public:
    BaseNode* FindNode(const std::string& nodePath);
    BaseNode* FindNode(const char * pNodePath);

    template<typename TNode, typename ...Args>
    TNode* AddChild(Args&&... params)
    {
        TNode* retVal = new TNode(std::forward<Args>(params)...);
        std::unique_ptr<BaseNode> newNode(retVal);
        newNode->pParent = this;        
        children.push_back(std::move(newNode));
        return retVal;
    }

    void SetNodeChanged();
    void Print(int indentLevel = 0) const;

    static void GetOptions(std::string& strOptions, const std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptions(const char* pValues, std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptionValue(const char* pValue, const std::vector<std::string> & options, int &value);
};

