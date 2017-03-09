#pragma once
#include "../nodes/Nodes.hpp"

class Module
{
public:
    virtual void Init(BaseNode& rootNode) {};
    virtual void Execute() = 0;
};