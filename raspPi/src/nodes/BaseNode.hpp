#pragma once
#include <string>
#include <vector>
#include <memory>

enum class NodeType { nodeNode, boolNode, intNode, floatNode, doubleNode, stringNode, enumNode };


class BaseNode
{
    typedef std::shared_ptr<BaseNode> TChildPtr;
protected:
    std::string name;
    NodeType nodeType;
    BaseNode* pParent;
    std::vector<TChildPtr> children;

public:
    BaseNode(const std::string &nodeName, BaseNode* pParentNode = nullptr, NodeType type = NodeType::nodeNode)
    {
        nodeType = type;
        pParent = pParentNode;
        name = nodeName;
    }

    virtual ~BaseNode()
    {

    }

    BaseNode* FindNode(std::string nodePath);
};