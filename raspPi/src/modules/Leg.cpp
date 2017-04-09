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
    pNodeServoAngle = { nullptr };
}
void Leg::Init(BaseNode& rootNode, int legNumber)
{
    legID = legNumber;
    string legNodeName = std::to_string(legNumber);
    pNodeLeg = rootNode.FindOrCreateChild<BaseNode>(legNodeName);
    if (pNodeLeg == nullptr)
    {
        // ToDo error.
    }

    pNodeLeg->Subscribe(this);

    pNodeMountPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("mountPosistion");
    pNodeMountPos->SetAttribute(unitAttrib.GetID(), "m");

    pNodeMountSide = pNodeLeg->FindOrCreateChild<BoolNode>("mountSide");
    pNodeMountSide->SetAttribute(optionsAttrib.GetID(), "left,right");

    pNodeGoalPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("goalPos");
    pNodeGoalPos->SetAttribute(unitAttrib.GetID(), "m");
    
    pNodeCurrentPos = pNodeLeg->FindOrCreateChild<Pos3D_32f_Node>("currentPos");
    pNodeCurrentPos->SetAttribute(unitAttrib.GetID(), "m");

    pNodeServoPath = pNodeLeg->FindOrCreateChild<StringNode>("servoPath","/Hardware/Servos/");

    BaseNode* pJointParent = pNodeLeg->FindOrCreateChild("joints");
    for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
    {
        string jointNodeName = std::to_string(jointIdx);
        pNodeJoints[jointIdx] = pJointParent->FindOrCreateChild<BaseNode>(jointNodeName);

        pNodeServoID[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<UInt16Node>("ServoID", (uint16_t)(legID * numJoints + jointIdx));

        pNodeJointAngles[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("jointAngle",0.0f,-90.0f, 90.f);
        pNodeJointAngles[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");

        pNodeJointDistance[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("jointDistance");
        pNodeJointDistance[jointIdx]->SetAttribute(unitAttrib.GetID(), "m");

        pNodeLinkAngle[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("linkAngle");
        pNodeLinkAngle[jointIdx]->SetAttribute(unitAttrib.GetID(), "deg");

        pNodeLinkDistance[jointIdx] = pNodeJoints[jointIdx]->FindOrCreateChild<FloatNode>("linkDistance");
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

    // ToDo: Implement mirrors instead.
    string servorPath;
    for (size_t jointIdx = 0; jointIdx < pNodeJoints.size(); ++jointIdx)
    {
        if (pNodeServoAngle[jointIdx]) continue;
        servorPath = pNodeServoPath->Get() + to_string(pNodeServoID[jointIdx]->Get());
        servorPath += "/SetAngle";
        pNodeServoAngle[jointIdx] = pNodeLeg->FindNode<FloatNode>(servorPath);

        if (pNodeJointAngles[jointIdx]->IsValueChanged() && pNodeServoAngle[jointIdx])
        {
            // Copy joint angle to servo angle.
            pNodeServoAngle[jointIdx]->Set(pNodeJointAngles[jointIdx]->Get());
        }
    }
}

void Leg::Execute()
{

}