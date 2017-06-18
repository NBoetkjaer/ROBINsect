#pragma once

#include "Module.hpp"
#include "../communication/Socket.hpp"
#include <array>
#include <string>

class DiscoveryModule : public Module, public NodeObserver
{
private:
    BaseNode* pDiscoveryNode;
    UInt16Node* pPortNode;
    BoolNode* pBoundNode;
public:
    DiscoveryModule();
    virtual ~DiscoveryModule();
    virtual void Publish() override;
    virtual void OnTimer() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
    enum class  State {Initialize, Bound};
    State state;
    Socket sockDiscover;
    std::array<char, 1024> buffer;

    // Inherited via NodeObserver
    virtual void Notify() override;
};