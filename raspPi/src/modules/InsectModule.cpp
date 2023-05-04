#include "InsectModule.hpp"
#include "gait/RippleGait.hpp"

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

    pNodeBodyPosZ = pNodeInsect->FindOrCreateChild<FloatNode>("bodyPosZ", 0.10f, 0.05f, 0.17f);
    pNodeBodyPosZ->SetAttribute(unitAttrib.GetID(), "m");

    pNodeStepLength = pNodeInsect->FindOrCreateChild<FloatNode>("step_length", 0.03f, 0.0f, 0.06f);
    pNodeStepLength->SetAttribute(unitAttrib.GetID(), "m");

    pNodeOrientation = pNodeInsect->FindOrCreateChild<FloatNode>("walk_orientation", 0.0f, -180.0f, 180.0f);
    pNodeOrientation->SetAttribute(unitAttrib.GetID(), "deg");

    pNodeVelocity = pNodeInsect->FindOrCreateChild<FloatNode>("velocity", 0.0f, -0.04f, 0.04f);
    pNodeVelocity->SetAttribute(unitAttrib.GetID(), "m/s");

    pNodeAngularVelocity = pNodeInsect->FindOrCreateChild<FloatNode>("angularVelocity", 0.0f, -5.0f, 5.0f);
    pNodeAngularVelocity->SetAttribute(unitAttrib.GetID(), "deg/s");

    for(int legIdx = 0; legIdx < legs.size(); ++legIdx)
    {
        legs[legIdx].CreateNodes(*pNodeLegs, legIdx);
    }
}

void InsectModule::LookupNodes()
{
    pNodeShutdown = pNodeInsect->FindNode<BoolNode>("/SystemInfo/ShutdownSystem", false);
    if (pNodeShutdown)
        pNodeShutdown->Subscribe(this);

    // Create test trajectory.
    //float stepDuration = 4.0f;
    //for (auto& leg : legs)
    //{

    //    Eigen::Vector3f step(0.08f, 0.08f, 0.0f);
    //    float dir = -1.0f;
    //    auto leg_ypos = 1 - (leg.GetLegID() / 2); // [1,0,-1]

    //    if (IS_RIGHT_LEG(leg.GetLegID())) // Left side move the other way.
    //    {
    //        dir *= -1.0f;
    //        step *= -1.0f;
    //    }
    //    Eigen::Vector3f pos(dir * 0.12f, leg_ypos * 0.15f, -0.13f);

    //    
    //    if (leg.GetLegID() < 3)
    //    {
    //        Eigen::Vector3f center(0.0f, 0.0f, -0.13f);
    //        std::unique_ptr<TrajectorySegment> traj = Gait::CreateCircularStepTrajectory(center, pos, dir * Eigen::Vector3f::UnitZ(), 0.5f, dir * 0.02f, stepDuration);
    //        leg.SetTrajectory(std::move(traj));
    //    }
    //    else
    //    {
    //        std::unique_ptr<TrajectorySegment> traj = Gait::CreateLinearStepTrajectory(step, pos, 0.02f, stepDuration);
    //        leg.SetTrajectory(std::move(traj));
    //    }        

    //    float legOffset = 1 / 6.0f;
    //    // Front legs. (even leg is on left side)
    //    leg.RestartTrajectory(stepDuration * legOffset * leg.GetLegID());

    //}

    InitializeGait();
    //// Front legs. (even leg is on left side)
    //legs[0].RestartTrajectory(stepDuration * 0.00f);
    //legs[1].RestartTrajectory(stepDuration * 0.25f);
    //// Middle legs.
    //legs[2].RestartTrajectory(stepDuration * 0.25f); //
    //legs[3].RestartTrajectory(stepDuration * 0.25f); //
    //// Back legs.
    //legs[4].RestartTrajectory(stepDuration * 0.50f);
    //legs[5].RestartTrajectory(stepDuration * 0.75f);

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
    if (pNodeTestTrajectory->IsValueChanged())
    {
        if (pNodeTestTrajectory->Get())
        {
            if(gait) gait->Start();
        }
        else
        {
            if (gait) gait->Stop();
        }
    }

    // Update walking gait parameters.
    if (pNodeStepLength->IsValueChanged())
    {
        if (gait) gait->SetStepLength(pNodeStepLength->Get());
    }
    if(pNodeOrientation->IsValueChanged())
    {
        if (gait) gait->SetStepDirection(DEG2RAD(pNodeOrientation->Get()));
    }
    if (pNodeVelocity->IsValueChanged())
    {
        if (gait) gait->SetVelocity(pNodeVelocity->Get());
    }
    if (pNodeAngularVelocity->IsValueChanged())
    {
        if (gait) gait->SetAngularVelocity(DEG2RAD(pNodeAngularVelocity->Get()));
    }
}

void InsectModule::Execute()
{
    if (gait != nullptr)
    {
        if (gait->GetState() == GaitStates::Stopped)
        {
            // Transfer initial leg positions
            for (auto& leg : legs)
            {
                float x, y, z;
                leg.GetCurrentPos(x, y, z);
                gait->SetLegPosition(leg.GetLegID(), { x, y, z });
            }
        }
        
        gait->UpdateGait(elapsed_us / 1e6f);

        if (gait->GetState() != GaitStates::Stopped)
        {
            for (auto& leg : legs)
            {
                Eigen::Vector3f pos;
                gait->GetLegPosition(leg.GetLegID(), pos);
                leg.SetGoal(pos[0], pos[1], pos[2]);
            }
        }
    }

    //for (auto& leg : legs)
    //{
    //    if (pNodeTestTrajectory->Get())
    //    {
    //        if (!leg.UpdateTrajectory(elapsed_us / 1e6f ))
    //        {
    //            leg.RestartTrajectory();
    //        }
    //    }
    //    //leg.Execute();
    //    //leg.SetGoal();
    //}
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

    if (initLeg >= NUM_LEGS)
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


void InsectModule::InitializeGait()
{
    gait = std::make_unique<RippleGait>();
    for (auto& leg : legs)
    {
        float dir = IS_RIGHT_LEG(leg.GetLegID()) ? 1.0f : -1.0f;
        auto leg_ypos = 1 - (leg.GetLegID() / 2); // [1,0,-1] (front, mid, back)

        Eigen::Vector3f pos(dir * 0.13f, leg_ypos * 0.15f, -0.13f); // ToDo Calculate - how??
        leg.SetGoal(pos[0], pos[1], pos[2]);
        gait->SetLegRange(leg.GetLegID(), pos, 0.03f);
    }
}
