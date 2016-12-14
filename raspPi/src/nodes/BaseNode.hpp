#pragma once
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <memory>
#include <iostream>
#include <typeinfo>
#include "Attribute.hpp"

enum class NodeType { nodeNode, boolNode, intNode, floatNode, doubleNode, stringNode, enumNode };

//flags ?? logging|hidden|readonly|persistent|query ...

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
    bool isChanged;
    size_t changes; // Variable to hold the applied changes to this node. Each attribID has a bit in changes.
    //std::map<size_t, string> attributes; key<attribID,string>
    std::vector<NodeObserver*> subscribers;
protected:
    std::string name;
    NodeType nodeType;
    BaseNode* pParent;
    std::vector<std::unique_ptr<BaseNode>> children;
    void Notify();
public:
    BaseNode(const std::string &nodeName, BaseNode* pParentNode = nullptr, NodeType type = NodeType::nodeNode);
    virtual ~BaseNode();

    // Sets the given attribute, based on the string argument.
    // Inherited nodes should call and return a base implementation if it does not handle the attributeID.
    // Returns true is the attribute is handeled/altered. 
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue);

    void Subscribe(NodeObserver* pObserver);
    void UnSubscribe(NodeObserver* pObserver);

    BaseNode* FindNode(std::string nodePath);

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

template <typename T>
class NumericNode : public GenericNode<T>
{
public:
    NumericNode(const std::string &nodeName, T val) : GenericNode<T> (nodeName, val){ }
    virtual ~NumericNode(){}

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

typedef GenericNode<bool> BoolNode;
typedef GenericNode<std::string> StringNode;
typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<float> FloatNode;