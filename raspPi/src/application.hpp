#pragma once
#include "nodes/Nodes.hpp"
#include "modules/Module.hpp"
#include "nodes/xml/nodeXmlConverter.hpp"

class Application
{
public:
    Application();
    virtual ~Application();
    void RunLoop();
    void LoadConfig();
    void SaveConfig();

private:
    BaseNode root;
    NodeXmlConverter xmlConverter;
};