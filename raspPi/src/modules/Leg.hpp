#pragma once
#include "Module.hpp"
#include  "LegKinematic.hpp"

class Leg : public NodeObserver
{
public:
    Leg();
    virtual ~Leg() {};

    void Init(BaseNode& rootNode, int legNumber);
    // Inherited via NodeObserver
    virtual void Notify() override;

private:
    static const int numJoints = 3;
    BaseNode* pNodeLeg;
    Pos3D_32f_Node* pNodeMountPos;
    std::array<BaseNode*, numJoints> pNodeJoints;
    std::array<FloatNode*, numJoints> pNodeCurrentAngle;
    std::array<FloatNode*, numJoints> pNodeJointAngles;
    std::array<FloatNode*, numJoints> pNodeJointDistance;
    std::array<FloatNode*, numJoints> pNodeLinkAngle;
    std::array<FloatNode*, numJoints> pNodeLinkDistance;
    LegKinematic kinematic;
};