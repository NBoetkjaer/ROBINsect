#pragma once
#include <vector>
#include "nodes/Nodes.hpp"
#include "modules/Module.hpp"


class Application
{
public:
    Application();
    virtual ~Application();
    void RunLoop();
private:
    std::vector<std::unique_ptr<Module>> modules;

    BaseNode root;
};