#pragma once

#include "Module.hpp"
#include "../nodes/Nodes.hpp"
#include "../nodes/xml/nodeXmlConverter.hpp"

class SystemModule : public Module
{
public:
    SystemModule();
    virtual ~SystemModule();

    virtual void Execute() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void LookupNodes() override;
    virtual void OnTimer() override;

    float GetLoopFrequency() const { return pLoopFreqNode->Get(); }
    void LoadConfig();
    void SaveConfig();
private:
    bool shutingdown = false;
    void Shutdown();

    NodeXmlConverter xmlConverter;

    BaseNode* pSystemNode;
    BoolNode* pShutdownNode;
    FloatNode* pLoopFreqNode;
    FloatNode* pActualLoopFreqNode;
    Int64Node* pLoopCountNode;
    BoolNode* pSaveConfig;
    BoolNode* pLoadConfig;
    
    int64_t loopCount = 0;
    float loopFreqAvg_Hz = 0;   
};