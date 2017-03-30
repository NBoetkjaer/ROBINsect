#pragma once
#include "../nodes/Nodes.hpp"

class Module
{
public:
    virtual void Init(BaseNode& rootNode) {};
    virtual void Execute() = 0; // May add info into the tree
    virtual void Publish() {}; // May not add info to the tree
};