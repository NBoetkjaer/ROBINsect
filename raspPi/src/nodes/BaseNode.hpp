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

//enum class NodeType { nodeNode, boolNode, intNode, floatNode, doubleNode, stringNode, enumNode };

extern Attribute valueAttrib("value");

enum class FlagType :uint32_t {hide, readonly, logging, persist, query, numflags, invalidFlag = numflags};
ENABLE_BITMASK_OPERATORS(FlagType) // Supply overloaded bitwise operators for FlagType.

// Attributes: Value, Type, Range, flags, Enums, DisplayName, DisplayFormat, unit, prefix, info, ... 


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

    // Sets the given attribute, based on the string argument.
    // Inherited nodes should call and return a base implementation if it does not handle the attributeID.
    // Returns true is the attribute is handeled. 
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue);
    // Mark a given attribute as changed.
    inline void SetAttributeChanged(attribID_t attribID){ changes |= 1 << attribID; }
	inline bool IsAttributeChanged(attribID_t attribID) { return (changes &= 1 << attribID)!=0;}

	// -------- Atributes --------
private:
	FlagType nodeFlag;
	std::string info;
public:
    // Flag attribute methods.
    void SetFlags(const char* pValues);
    void SetFlag(FlagType flag, bool value);
    bool GetFlag(FlagType flag);
	// Info attributes methods.
	const std::string &GetInfo() {return info;}
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

    void SetValueChanged();
    void Print(int indentLevel = 0) const; 
};

template <typename T>
class GenericNode : public BaseNode
{
public:
    GenericNode(const std::string &nodeName, T val) : BaseNode (nodeName), value(val) { }
    virtual ~GenericNode(){}

    virtual void Set(const T &newValue)
    { 
        if(newValue == value) return;
        value = newValue;
        SetValueChanged();
    }
    T Get() const { return value;}    
private:
    T value;
};



typedef GenericNode<bool> BoolNode;
typedef GenericNode<std::string> StringNode;
