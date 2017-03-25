#include "Leg.hpp"
#include <string>

using namespace std;

Leg::Leg()
{
    pNodeLeg = nullptr;
    pNodeMountPos = nullptr;
    pNodeJoints = { nullptr };
    pNodeCurrentAngle = { nullptr };
    pNodeJointAngles = { nullptr };
    pNodeJointDistance = { nullptr };
    pNodeLinkAngle = { nullptr };
    pNodeLinkDistance = { nullptr };
}
void Leg::Init(BaseNode& rootNode, int legNumber)
{
    string legNodeName = "Leg" + std::to_string(legNumber);
    pNodeLeg = rootNode.AddChild<BaseNode>(legNodeName);
    pNodeLeg->Subscribe(this);
    pNodeMountPos = pNodeLeg->AddChild<Pos3D_32f_Node>("MountPosistion");
    for (int jointIdx = 0; jointIdx < numJoints; ++jointIdx)
    {
        string jointNodeName ="joint" + std::to_string(jointIdx);
        pNodeJoints[jointIdx] = pNodeLeg->AddChild<BaseNode>(jointNodeName);
        
        pNodeCurrentAngle[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("currentAngle");
        pNodeCurrentAngle[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");
        pNodeCurrentAngle[jointIdx]->SetRange(-90.0f, 90.0f);

        pNodeJointAngles[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("jointAngle");
        pNodeJointAngles[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");

        pNodeJointDistance[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("jointDistance");
        pNodeJointDistance[jointIdx]->SetAttribute(unitAttrib.GetID(), "m");

        pNodeLinkAngle[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("linkAngle");
        pNodeLinkAngle[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");

        pNodeLinkDistance[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("linkDistance");
        pNodeLinkDistance[jointIdx]->SetAttribute(unitAttrib.GetID(), "m");
    }
}

void Leg::Notify()
{
}