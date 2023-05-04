#pragma once
#include <memory>
#include "Module.hpp"
#include "LegKinematic.hpp"
#include "trajectory/TrajectorySegment.hpp"
#include  <Eigen/Dense>

class Leg : public NodeObserver
{
public:
    Leg();
    virtual ~Leg() {};
    uint16_t GetLegID()const { return legID;}
    void CreateNodes(BaseNode& rootNode, int legNumber);
    //void Execute();
    // Inherited via NodeObserver
    virtual void Notify() override;
    
    void SetGoal(float x, float y, float z);
    void GetCurrentPos(float &x, float &y, float &z) const;

    void EnableLeg(bool enable);
    const LegKinematic& GetKinematic() const { return kinematic; };
    void RestartTrajectory(float initialTime_s = 0.0f) { deltaTime_s = initialTime_s;}
    const TrajectorySegment* GetTrajectory() { return pTrajectory.get();}
    void SetTrajectory(std::unique_ptr<TrajectorySegment> pNewTrajectory)
    {
        pTrajectory = std::move(pNewTrajectory);
        deltaTime_s = 0.0f;
    }
    bool UpdateTrajectory(float elapsedTime_s)
    {
        if (pTrajectory != nullptr)
        {
            deltaTime_s += elapsedTime_s;
            if(deltaTime_s < pTrajectory->GetDuration())
            {
                Eigen::Vector3f pos;
                pTrajectory->GetPosition(deltaTime_s, pos);
                pNodeGoalPos->SetPosition(pos[0], pos[1], pos[2]);
                return true;
            }
        }
        return false;
    }
private:
    void setDefaultValues();
    uint16_t legID;
    static const int numJoints = 3;
    BaseNode* pNodeLeg;
    BoolNode* pNodeEnable;
    Pos3D_32f_Node* pNodeMountPos;
    Pos3D_32f_Node* pNodeMountAngle_deg;
    Pos3D_32f_Node* pNodeGoalPos;
    Pos3D_32f_Node* pNodeCurrentPos;

    std::array<BaseNode*, numJoints> pNodeJoints;
    std::array<FloatNode*, numJoints> pNodeJointAngles;
    std::array<FloatNode*, numJoints> pNodeJointDistance;
    std::array<FloatNode*, numJoints> pNodeLinkAngle;
    std::array<FloatNode*, numJoints> pNodeLinkDistance;
    std::array<BoolNode*, numJoints> pNodeJointEnabled;

    LegKinematic kinematic;


    std::unique_ptr<TrajectorySegment> pTrajectory;
    float deltaTime_s = 0;
    std::unique_ptr<Eigen::AffineCompact3f> toLegT = std::make_unique<Eigen::AffineCompact3f>();
};