#include "Leg.hpp"
#include <string>

using namespace std;

void Leg::Init(BaseNode& rootNode, int legNumber)
{
    string legNodeName = "Leg" + std::to_string(legNumber);
    pNodeLeg = rootNode.AddChild<BaseNode>(legNodeName);
    pNodeLeg->Subscribe(this);
    pNodeMountPos = pNodeLeg->AddChild<Pos3D_32f_Node>("MountPosistion");
    for (int jointIdx = 0; jointIdx < numJoints; ++jointIdx)
    {
        string jointNodeName ="joint" + std::to_string(jointIdx);
        pNodeLeg->AddChild<BaseNode>(jointNodeName);
    }
}

void Leg::Notify()
{
}