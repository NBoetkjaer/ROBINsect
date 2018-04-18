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

        float linkDist = 0.086f;
        float linkAngle = 0;
        switch(jointIdx)
        {
        case 0:
            linkDist = 0.0597f;
            pNodeJointAngles[jointIdx]->SetRange(-90.0f, 70.0f);
            linkAngle = legID & 0x1 ? -90.0f : 90.0f;
            break;
        case 1:
            linkDist = 0.08603f;
            linkAngle = 180;
            break;
        case 2:
            linkDist = 0.09025f;
            break;
        }
        pNodeLinkDistance[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("linkDistance", linkDist);
        pNodeLinkDistance[jointIdx]->SetAttribute(unitAttrib.GetID(), "m");
        pNodeLinkDistance[jointIdx]->SetFlag(FlagType::persist, true, false);
        pNodeLinkAngle[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("linkAngle", linkAngle);
        pNodeLinkAngle[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");
        pNodeLinkAngle[jointIdx]->SetFlag(FlagType::persist, true, false);
    }
}

void Leg::Notify()
{
    if(pNodeMountPos->AnyChanges())
    {

        // Update Body -> Leg transformation.
    }

    if (pNodeGoalPos->AnyChanges())
    {
        // Calculate a new trajectory.
        float x, y, z;
        pNodeGoalPos->GetPosition(x, y, z);
        Eigen::Vector3f goalPos = { x, y, z };
        Eigen::Vector3f jointAngles_deg;
        if (kinematic.getJointAngles(goalPos, jointAngles_deg)) // Is goal position reachable.
        {
            //std::cout << "joint space: " << jointAngles_deg.transpose() << std::endl;
            pNodeCurrentPos->SetPosition(x, y, z);
            for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
            {
                pNodeJointAngles[jointIdx]->Set(jointAngles_deg[jointIdx]);
            }
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
        if (pNodeLinkAngle[jointIdx]->IsValueChanged())
        {
            kinematic.SetDH_LinkAngle(jointIdx, DEG2RAD(pNodeLinkAngle[jointIdx]->Get()));
        }
    }
}

void Leg::SetGoal(float x, float y, float z)
{
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