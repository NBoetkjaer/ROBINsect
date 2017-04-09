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
    std::vector<std::unique_ptr<Module>> modules;

    BaseNode root;
    BaseNode* pSystemNode;
    FloatNode* pLoopFreqNode;
    FloatNode* pActualLoopFreqNode;
    Int64Node* pLoopCountNode;
    BoolNode* pSaveConfig;
    BoolNode* pLoadConfig;

    NodeXmlConverter xmlConverter;
};