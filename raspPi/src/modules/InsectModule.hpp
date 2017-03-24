#pragma once
#include "Module.hpp"

#include <array>
#include  "Leg.hpp"

class InsectModule : public Module, public NodeObserver
{
public:
    InsectModule();
    virtual ~InsectModule();
    virtual void Execute();
    virtual void Init(BaseNode& rootNode) override;
private:
    static const int numLegs = 6;
    // Inherited via NodeObserver
    virtual void Notify() override;

    // Nodes
    BaseNode* pNodeInsect;
    std::array<Leg, numLegs> legs;
    Pos3D_32f_Node* pNodePosition;
};