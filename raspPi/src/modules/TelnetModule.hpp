#pragma once

#include "Module.hpp"
#include "../communication/Socket.hpp"
#include "../cmd.h"
#include <array>
#include <string>

class TelnetModule : public Module, public NodeObserver
{
private:
    BaseNode* pNetworkNode;
    Int64Node* pRcvNode;
    Int64Node* pSentNode;
    BoolNode* pConnectedNode;
    BaseNode* pCurrentNode;
    std::string consoleOutput;
    bool updateOutput;
    bool clearConsole;
public:
    TelnetModule();
    virtual ~TelnetModule();
    virtual void Execute() override;
    virtual void Publish() override;
    virtual void OnTimer() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
    enum class  State {Initialize, Listning, Connected};
    State state;
    Socket sockDiscover;
    Socket sockListen;
    Socket sockAccept;

    std::array<char, 1024> buffer;
    size_t bufferWritePos;
    size_t bufferEndPos;

    void PrintNodes(bool clear);
    void ProcessCmd(const char* pCmd);

    // Inherited via NodeObserver
    virtual void Notify() override;
};