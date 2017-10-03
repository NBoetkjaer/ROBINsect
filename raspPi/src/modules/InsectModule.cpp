#include "InsectModule.hpp"
#include <cstdlib>
using namespace std;


InsectModule::InsectModule()
{
}

InsectModule::~InsectModule()
{
}

void InsectModule::CreateNodes(BaseNode& rootNode)
{
    pNodeInsect = rootNode.FindOrCreateChild<BaseNode>("Insect");
    pNodeInsect->Subscribe(this);

    pNodeLegs = pNodeInsect->FindOrCreateChild<BaseNode>("Legs");

    pNodeEnable = pNodeInsect->FindOrCreateChild<BoolNode>("Enable", false);
    pNodeSoftStart = pNodeInsect->FindOrCreateChild<BoolNode>("SoftStart", false);    
    pNodeTestTrajectory = pNodeInsect->FindOrCreateChild<BoolNode>("TestTrajectory", false);

    pNodePosition = pNodeInsect->FindOrCreateChild<Pos3D_32f_Node>("BodyPosition");
    for(size_t legIdx = 0; legIdx < legs.size(); ++legIdx)
    {
        legs[legIdx].CreateNodes(*pNodeLegs, legIdx);
    }
}
void InsectModule::LookupNodes()
{
    pNodeShutdown = pNodeInsect->FindNode<BoolNode>("/SystemInfo/ShutdownSystem", false);
    if (pNodeShutdown)
        pNodeShutdown->Subscribe(this);
}

void InsectModule::Notify()
{
    if (pNodeEnable->IsValueChanged())
    {
        EnableLegs(pNodeEnable->Get());
    }
    if (pNodeSoftStart->IsValueChanged() && pNodeSoftStart->Get() && !isSoftStarting)
    {
        if (pNodeEnable->Get())
        {   // Disable softstart when already enabled.
            pNodeSoftStart->Set(false);
        }
        else
        {
            SoftStart();
        }
    }
    if (pNodeShutdown->Get()) // When shutting down - disable all legs
    {
        EnableLegs(false);
    }
}

void InsectModule::Execute()
{
    Eigen::Vector3f start;
    Eigen::Vector3f end;
    static bool dir = false;
    bool updateTrajectory = false;
    for (auto& leg : legs)
    {
        if (pNodeTestTrajectory->Get())
        {
            start << 0.12f, -0.07f, -0.13f;
            end << 0.12f, 0.07f, -0.13f;
            if (leg.GetLegID() & 0x1) // Left side move the other way.
            {
               std::swap(start, end);
            }
            if (!leg.UpdateTrajectory(1.0f))
            {                
                if (dir)
                {
                    std::swap(start, end);
                }
                updateTrajectory = true;
                std::unique_ptr<TrajectorySegment> trajectory = std::make_unique<LinearTrajectorySegment>(start, end, 480.0f);
                leg.SetTrajectory(std::move(trajectory));
            }
        }
        //leg.Execute();
        //leg.SetGoal();
    }
    if (updateTrajectory)
    {
        dir = !dir;
    }
}

void InsectModule::OnTimer()
{
    if (isSoftStarting)
    {
        SoftStart();
    }
}

void InsectModule::EnableLegs(bool enable)
{
    for (auto& leg : legs)
    {
        leg.EnableLeg(enable);
    }
    pNodeEnable->Set(enable);
}

void InsectModule::SoftStart()
{
    if (!isSoftStarting)
    {   // Initialize softstart sequence.
        isSoftStarting = true;
        SetTimeout(500);
        pNodeSoftStart->Set(true);
        initLeg = 0;
    }

    if (initLeg >= numLegs)
    {   // Finalize softstart sequence.
        isSoftStarting = false;
        SetTimeout(NoTimeOut);
        pNodeEnable->Set(true);
        pNodeSoftStart->Set(false);
        return;
    }
    // Perform softstart step.
    legs[initLeg].EnableLeg(true);
    initLeg++;
}
