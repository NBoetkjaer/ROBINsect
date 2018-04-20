#pragma once

#include "Module.hpp"
//#include <array>
//#include <string>

class CommandLineModule : public Module, public NodeObserver
{
private:
    StringNode* pNodeCommand;
    StringNode* pNodeOutput;
public:
    CommandLineModule(){};
    virtual ~CommandLineModule(){};
    virtual void Publish() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
//    virtual void DataReceived(char* pData, size_t dataLen) override;
    void ProcessCmd(const char* pCmd);

    // Inherited via NodeObserver
    virtual void Notify() override;
};