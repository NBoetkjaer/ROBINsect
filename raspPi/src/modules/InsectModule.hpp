#pragma once
#include "Module.hpp"
#include  "Leg.hpp"
#include "gait/gait.hpp"

#include <array>
#include <memory>

class InsectModule : public Module, public NodeObserver
{
public:
    InsectModule();
    virtual ~InsectModule();
    virtual void Execute() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void LookupNodes() override;
    virtual void OnTimer() override;
private:
    // Inherited via NodeObserver
    virtual void Notify() override;
    
    void EnableLegs(bool enable);
    void SoftStart();
    void InitializeGait();
    bool isSoftStarting = false;
    int initLeg = 0;

    std::unique_ptr<Gait> gait;
    
    // Nodes
    BaseNode* pNodeInsect = nullptr;
    BaseNode* pNodeLegs = nullptr;
    std::array<Leg, NUM_LEGS> legs;
    
    BoolNode* pNodeEnable = nullptr;
    BoolNode* pNodeSoftStart = nullptr;
    BoolNode* pNodeShutdown = nullptr;
    BoolNode* pNodeTestTrajectory = nullptr;
    
    FloatNode* pNodeBodyPosZ = nullptr;
    //Pos3D_32f_Node* pNodeBodyAttitude = nullptr;
    FloatNode* pNodeStepLength = nullptr;
    FloatNode* pNodeOrientation = nullptr;
    FloatNode* pNodeVelocity = nullptr;
    FloatNode* pNodeAngularVelocity = nullptr;

};