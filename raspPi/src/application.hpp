#pragma once
#include <vector>
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
    BaseNode* pSystemNode;
    FloatNode* pLoopFreqNode;
    FloatNode* pActualLoopFreqNode;
    Int64Node* pLoopCountNode;

    NodeXmlConverter xmlConverter;
};