#include "Leg.hpp"
#include <string>

using namespace std;

Leg::Leg()
{
    pNodeLeg = nullptr;
    pNodeMountPos = nullptr;
    pNodeGoalPos = nullptr;
    pNodeCurrentPos = nullptr;
    pNodeMountSide = nullptr;
    pNodeJoints = { nullptr };
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
    pNodeMountPos->SetAttribute(unitAttrib.GetID(), "m");

    pNodeMountSide = pNodeLeg->AddChild<BoolNode>("MountSide");
    pNodeMountSide->SetAttribute(optionsAttrib.GetID(), "Left,Right");

    pNodeGoalPos = pNodeLeg->AddChild<Pos3D_32f_Node>("GoalPos");
    pNodeGoalPos->SetAttribute(unitAttrib.GetID(), "m");
    
    pNodeCurrentPos = pNodeLeg->AddChild<Pos3D_32f_Node>("CurrentPos");
    pNodeCurrentPos->SetAttribute(unitAttrib.GetID(), "m");

    for (int jointIdx = 0; jointIdx < numJoints; ++jointIdx)
    {
        string jointNodeName ="joint" + std::to_string(jointIdx);
        pNodeJoints[jointIdx] = pNodeLeg->AddChild<BaseNode>(jointNodeName);

        pNodeJointAngles[jointIdx] = pNodeJoints[jointIdx]->AddChild<FloatNode>("jointAngle");
        pNodeJointAngles[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");
        pNodeJointAngles[jointIdx]->SetRange(-90.0f, 90.0f);

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
    if(pNodeMountPos->IsValueChanged() || pNodeMountSide->IsValueChanged())
    {
        // Update Body -> Leg transformation.
    }

    if (pNodeGoalPos->IsValueChanged())
    {
        // Calculate a new trajectory.
    }
}

void Leg::Execute()
{

}