#pragma once
#include "Module.hpp"
#include  "LegKinematic.hpp"

class Leg : public NodeObserver
{
public:
    void Init(BaseNode& rootNode, int legNumber);
    // Inherited via NodeObserver
    virtual void Notify() override;

private:
    static const int numJoints = 3;
    BaseNode* pNodeLeg;
    Pos3D_32f_Node* pNodeMountPos;
    std::array<BaseNode*, numJoints> pNodeJoints;
    LegKinematic kinematic;
};