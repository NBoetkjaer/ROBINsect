#include "Leg.hpp"
#include <string>
#include <Eigen/Dense>
#include "constants.hpp"

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
    pNodeMountPos->SetFlag(FlagType::persist, true, false);

    pNodeMountAngle_deg = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("mountAngle");
    pNodeMountAngle_deg->SetAttribute(unitAttrib.GetID(), "deg");
    pNodeMountAngle_deg->SetFlag(FlagType::persist, true, false);
    setDefaultValues();

    pNodeGoalPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("goalPos");
    pNodeGoalPos->SetAttribute(unitAttrib.GetID(), "m");
    
    pNodeCurrentPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("currentPos");
    pNodeCurrentPos->SetAttribute(unitAttrib.GetID(), "m");
    pNodeCurrentPos->SetFlag(FlagType::readonly, true, false);

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
            pNodeJointAngles[jointIdx]->SetRange(-70.0f, 70.0f);
            linkAngle = legID & 0x1 ? -90.0f : 90.0f;
            if (legID == 4) linkAngle = -90;
            if (legID == 5) linkAngle = 90;
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

void Leg::setDefaultValues()
{
    switch (legID)
    {
    case LEG_FRONT_LEFT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 180.0f);
        pNodeMountPos->SetPosition(-0.04301f, 0.15f, 0.0f);
        break;
    case LEG_FRONT_RIGHT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 0.0f);
        pNodeMountPos->SetPosition(0.04301f, 0.15f, 0.0f);
        break;
    case LEG_MID_LEFT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 180.0f);
        pNodeMountPos->SetPosition(-0.04301f, 0.0f, 0.0f);
        break;
    case LEG_MID_RIGHT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 0.0f);
        pNodeMountPos->SetPosition(0.04301f, 0.0f, 0.0f);
        break;
    case LEG_BACK_LEFT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 180.0f);
        pNodeMountPos->SetPosition(-0.04301f, -0.15f, 0.0f);
        break;
    case LEG_BACK_RIGHT:
        pNodeMountAngle_deg->SetPosition(0.0f, 0.0f, 0.0f);
        pNodeMountPos->SetPosition(0.04301f, -0.15f, 0.0f);
        break;
    default:
        break;
    }
}

void Leg::Notify()
{
    bool recalculatePos = false;
    if(pNodeMountPos->IsValueChanged() || pNodeMountAngle_deg->IsValueChanged())
    {
        // Update Body -> Leg transformation.
        float x, y, z, x_deg, y_deg, z_deg;
        pNodeMountAngle_deg->GetPosition(x_deg, y_deg, z_deg);
        pNodeMountPos->GetPosition(x, y, z);
        // Fixed angle convention (Roll, Pitch, yaw) 
        auto fromLegT = Eigen::Translation3f(x, y, z) * 
                      Eigen::AngleAxisf(DEG2RAD(z_deg), Eigen::Vector3f::UnitZ()) *
                      Eigen::AngleAxisf(DEG2RAD(y_deg), Eigen::Vector3f::UnitY()) *
                      Eigen::AngleAxisf(DEG2RAD(x_deg), Eigen::Vector3f::UnitX());
        *toLegT = fromLegT.inverse();
        recalculatePos = true;
    }

    for (int jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
    {
        if (pNodeLinkDistance[jointIdx]->IsValueChanged())
        {
            kinematic.SetDH_LinkDist(jointIdx, pNodeLinkDistance[jointIdx]->Get());
            recalculatePos = true;
        }
        if (pNodeLinkAngle[jointIdx]->IsValueChanged())
        {
            kinematic.SetDH_LinkAngle(jointIdx, DEG2RAD(pNodeLinkAngle[jointIdx]->Get()));
            recalculatePos = true;
        }
    }

    if (pNodeGoalPos->IsValueChanged())
    {
        // Calculate a new trajectory.
        float x, y, z;
        pNodeGoalPos->GetPosition(x, y, z);
        Eigen::Vector3f goalPos = (*toLegT) * Eigen::Vector3f(x, y, z);
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

    if (recalculatePos)
    {        
        Eigen::Vector3f jointAngles_deg;
        Eigen::Vector3f tipPos;
        for (size_t jointIdx = 0; jointIdx < pNodeJointAngles.size(); ++jointIdx)
        {
            jointAngles_deg[jointIdx] = pNodeJointAngles[jointIdx]->Get();
        }
        kinematic.getTipPosition(jointAngles_deg, tipPos);
        tipPos = toLegT->inverse() * tipPos;
        pNodeCurrentPos->SetPosition(tipPos[0], tipPos[1], tipPos[2]);
    }
}

void Leg::SetGoal(float x, float y, float z)
{
    pNodeGoalPos->SetPosition(x, y, z);
}

void Leg::GetCurrentPos(float& x, float& y, float& z) const
{
    pNodeCurrentPos->GetPosition(x, y, z);
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