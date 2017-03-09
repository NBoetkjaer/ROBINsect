#pragma once

#include "Module.hpp"
#include "../communication/Socket.hpp"
#include <array>

class NetworkModule : public Module, public NodeObserver
{
private:
    BaseNode* pNetworkNode;
    Int64Node* pRcvNode;
    Int64Node* pSentNode;
    BoolNode* pConnectedNode;
public:
    NetworkModule();
    virtual ~NetworkModule();
    virtual void Execute();
    virtual void Init(BaseNode& rootNode) override;
private:
    enum class  State {Initialize, Listning, Connected};
    State state;
    Socket sockDiscover;
    Socket sockListen;
    Socket sockAccept;
    std::array<char, 1024> buffer;

    // Inherited via NodeObserver
    virtual void Notify() override;
};