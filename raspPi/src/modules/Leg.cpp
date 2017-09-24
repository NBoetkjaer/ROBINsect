#include "Leg.hpp"
#include <string>
#include <Eigen/Dense>

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
void Leg::CreateNodes(BaseNode& rootNode, int legNumber)
{
    legID = legNumber;
    string legNodeName ="l" + std::to_string(legNumber);
    pNodeLeg = rootNode.FindOrCreateChild<BaseNode>(legNodeName);
    if (pNodeLeg == nullptr)
    {
        // ToDo error.
    }

    pNodeLeg->Subscribe(this);

    pNodeEnable = pNodeLeg->FindOrCreateChild<BoolNode>("enabled", false);

    pNodeMountPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("mountPosition");
    pNodeMountPos->SetAttribute(unitAttrib.GetID(), "m");

    pNodeMountSide = pNodeLeg->FindOrCreateChild<BoolNode>("mountSide");
    pNodeMountSide->SetAttribute(optionsAttrib.GetID(), "left,right");

    pNodeGoalPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("goalPos", 0.1f, 0.0f, -0.15f);
    pNodeGoalPos->SetAttribute(unitAttrib.GetID(), "m");
    
    pNodeCurrentPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("currentPos");
    pNodeCurrentPos->SetAttribute(unitAttrib.GetID(), "m");

    BaseNode* pJointParent = pNodeLeg->FindOrCreateChild("Joints");
    for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
    {
        string jointNodeName = "j" + std::to_string(jointIdx);
        pNodeJoints[jointIdx] = pJointParent->FindOrCreateChild<BaseNode>(jointNodeName);

        pNodeJointEnabled[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<BoolNode>("enabled", false);
        pNodeJointAngles[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("jointAngle", 0.0f, -105.0f, 105.f);
        pNodeJointAngles[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");

        float linkDist = 0.087f;
        if(jointIdx == 0)
        {
            linkDist = 0.06f;
            pNodeJointAngles[jointIdx]->SetRange(-90.0f, 70.0f);
        }

        pNodeLinkDistance[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("linkDistance", linkDist);
        pNodeLinkDistance[jointIdx]->SetAttribute(unitAttrib.GetID(), "m");
    }
}

void Leg::Notify()
{
    if(pNodeMountPos->AnyChanges() || pNodeMountSide->IsValueChanged())
    {
        if (!pNodeMountSide->Get()) // right side 
        {
            kinematic.SetDH_LinkAngle(DEG2RAD(90), DEG2RAD(180), 0.0f);
        }
        else
        { 
            kinematic.SetDH_LinkAngle(DEG2RAD(-90), DEG2RAD(180), 0.0f);
        }
        // Update Body -> Leg transformation.
    }

    if (pNodeGoalPos->AnyChanges())
    {
        // Calculate a new trajectory.
        float x, y, z;
        pNodeGoalPos->GetPosition(x, y, z);
        pNodeCurrentPos->SetPosition(x, y, z);
        kinematic.setGoal(x, y, z);
        for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
        {
            pNodeJointAngles[jointIdx]->Set(kinematic.getJoint(jointIdx));
        }
    }

    if (pNodeEnable->IsValueChanged())
    {
        EnableLeg(pNodeEnable->Get());
    }

    for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
    {
        if (pNodeLinkDistance[jointIdx]->IsValueChanged())
        {
            kinematic.SetDH_LinkDist(jointIdx, pNodeLinkDistance[jointIdx]->Get());
        }
    }
}

void Leg::SetGoal(float x, float y, float z)
{
    // Transform to the leg coordinate system.
    if (!pNodeMountSide->Get()) // right side 
    {
    }
    else
    {
    }
    pNodeGoalPos->SetPosition(x, y, z);
}


void Leg::EnableLeg(bool enable)
{
    pNodeEnable->Set(enable);
    for (auto node : pNodeJointEnabled)
    {
        node->Set(enable);
    }
}
//void Leg::Execute()
//{
//
//}