#pragma once
#include "Module.hpp"
#include "LegKinematic.hpp"
#include "TrajectorySegment.hpp"
#include  <Eigen/Dense>

class Leg : public NodeObserver
{
public:
    Leg();
    virtual ~Leg() {};

    void CreateNodes(BaseNode& rootNode, int legNumber);
    //void Execute();
    // Inherited via NodeObserver
    virtual void Notify() override;
    
    void SetGoal(float x, float y, float z);
    void EnableLeg(bool enable);
    const LegKinematic& GetKinematic() const { return kinematic; };
    void SetTrajectory(std::unique_ptr<TrajectorySegment> pNewTrajectory)
    {
        pTrajectory = std::move(pNewTrajectory);
        deltaTime = 0.0f;
    }
    bool UpdateTrajectory(float elapsedTime)
    {
        if (pTrajectory != nullptr)
        {
            deltaTime += elapsedTime;
            if(deltaTime < pTrajectory->GetDeltaTime())
            {
                Eigen::Vector3f pos;
                pTrajectory->GetPosition(deltaTime, pos);
                pNodeGoalPos->SetPosition(pos[0], pos[1], pos[2]);
                return true;
            }
        }
        return false;
    }
private:
    uint16_t legID;
    static const int numJoints = 3;
    BaseNode* pNodeLeg;
    BoolNode* pNodeEnable;
    Pos3D_32f_Node* pNodeMountPos;
    Pos3D_32f_Node* pNodeGoalPos;
    Pos3D_32f_Node* pNodeCurrentPos;
    BoolNode* pNodeMountSide;


    std::array<BaseNode*, numJoints> pNodeJoints;
    std::array<FloatNode*, numJoints> pNodeJointAngles;
    std::array<FloatNode*, numJoints> pNodeJointDistance;
    std::array<FloatNode*, numJoints> pNodeLinkAngle;
    std::array<FloatNode*, numJoints> pNodeLinkDistance;
    std::array<BoolNode*, numJoints> pNodeJointEnabled;

    LegKinematic kinematic;

    // ToDo: Temporary solution - consider using a list of segments.
    std::unique_ptr<TrajectorySegment> pTrajectory;
    float deltaTime = 0;
    //Eigen::Affine3f toLegT;
};