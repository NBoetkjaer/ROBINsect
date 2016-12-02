#pragma once
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <iostream>
#include <typeinfo>

enum class NodeType { nodeNode, boolNode, intNode, floatNode, doubleNode, stringNode, enumNode };

//flags ?? logging|hidden|readonly|persistent ...

class NodeObserver
{
public:
    virtual void Notify(){};
};

class BaseNode
{
private:
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

    void Subscribe(NodeObserver* pObserver);
    void UnSubscribe(NodeObserver* pObserver);

    BaseNode* FindNode(std::string nodePath);

    template<typename TNode, typename ...Args>
    BaseNode* AddChild(Args&&... params)
    {
        std::unique_ptr<BaseNode> newNode(new TNode(std::forward<Args>(params)...));
        newNode->pParent = this;
        children.push_back(std::move(newNode));
        return children.end()->get();
    }

    void Print() const 
    {
        std::cout << name <<"  " << typeid(*this).name() << std::endl;
        for(const auto &child : children)
        {
            child->Print();
        }
    }
};

template <typename T>
class NumericNode : public BaseNode
{
public:
    NumericNode(const std::string &nodeName, T val) : BaseNode (nodeName), value(val) { };
    virtual ~NumericNode(){};
private:
    T value;
};

typedef NumericNode<bool> BoolNode;
typedef NumericNode<int32_t> Int32Node;
typedef NumericNode<float> FloatNode;