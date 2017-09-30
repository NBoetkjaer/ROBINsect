#pragma once

#include "Module.hpp"
#include "TcpSocketModule.hpp"
#include "../cmd.h"
#include <array>
#include <string>

class TelnetModule : public TcpSocketModule, public NodeObserver
{
private:
    BaseNode* pCurrentNode;
    std::string consoleOutput;
    bool updateOutput;
    bool clearConsole;
public:
    TelnetModule();
    virtual ~TelnetModule();
    virtual void Publish() override;
    virtual void OnTimer() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
    virtual void DataReceived(char* pData, size_t dataLen) override;

    void PrintNodes(bool clear);
    void ProcessCmd(const char* pCmd, size_t dataLen);

    // Inherited via NodeObserver
    virtual void Notify() override;
};