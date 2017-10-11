#pragma once

#include "Module.hpp"
//#include <array>
//#include <string>

class CommandLineModule : public Module, public NodeObserver
{
private:
    BaseNode* pCommandsNode;
public:
    CommandLineModule(){};
    virtual ~CommandLineModule(){};
    virtual void Publish() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
//    virtual void DataReceived(char* pData, size_t dataLen) override;
    void ProcessCmd(const char* pCmd, size_t dataLen);

    // Inherited via NodeObserver
    virtual void Notify() override;
};