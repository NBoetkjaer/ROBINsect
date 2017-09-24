#pragma once
#include "Module.hpp"

#include <array>
#include  "Leg.hpp"

class InsectModule : public Module, public NodeObserver
{
public:
    InsectModule();
    virtual ~InsectModule();
    virtual void Execute() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void OnTimer() override;
private:
    static const int numLegs = 6;
    // Inherited via NodeObserver
    virtual void Notify() override;
    
    void EnableLegs(bool enable);
    void SoftStart();
    bool isSoftStarting = false;
    int initLeg;

    // Nodes
    BaseNode* pNodeInsect;
    BaseNode* pNodeLegs;
    std::array<Leg, numLegs> legs;
    Pos3D_32f_Node* pNodePosition;
    BoolNode* pNodeEnable;
    BoolNode* pNodeSoftStart;
    BoolNode* pNodeShutdown;

};