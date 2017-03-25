#pragma once
#include "Nodes.hpp"
#include <string>
#include <memory>

class NodeFactory
{
public:
    NodeFactory(){};
    virtual ~NodeFactory() = 0;
    static std::unique_ptr<BaseNode> CreateNode(const std::string& nodeType, const std::string& nodeName);
    static const std::string& NodeTypeName(const BaseNode* pNode);
};

