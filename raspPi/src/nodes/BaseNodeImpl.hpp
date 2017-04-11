#pragma once

#include "BaseNode.hpp"
#include "MirrorNode.hpp"

template<typename TNode>
TNode* BaseNode::AddChild(std::unique_ptr<BaseNode> childNode)
{
    TNode* retVal = dynamic_cast<TNode*>(childNode.get());
    childNode->pParent = this;
    children.push_back(std::move(childNode));
    SetNodeChanged();
    return retVal;
}

template<typename TNode, typename ...Args>
TNode* BaseNode::AddChild(Args&&... params)
{
    TNode* retVal = new TNode(std::forward<Args>(params)...);
    std::unique_ptr<BaseNode> newNode(retVal);
    newNode->pParent = this;    
    // ToDo: Validate node name (must be unique and no special characters which are not allowed in an XML tag name)
    //newNode->GetName()
    children.push_back(std::move(newNode));
    SetNodeChanged();
    return retVal;
}

template<typename TNode, typename ...Args>
TNode* BaseNode::FindOrCreateChild(const char * pNodeName, Args&&... params)
{
    BaseNode* childNode = FindChild(pNodeName);
    if (childNode)
    {
        if (typeid(*childNode) != typeid(TNode))
        {
            // Error unexpected type
        }
        return dynamic_cast<TNode*>(childNode);
    }
    return AddChild<TNode>(pNodeName, std::forward<Args>(params)...);
}