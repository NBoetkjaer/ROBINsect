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
    pNodeShutdown = pNodeInsect->FindOrCreateChild<BoolNode>("ShutdownSystem", false);

    pNodePosition = pNodeInsect->FindOrCreateChild<Pos3D_32f_Node>("BodyPosition");
    for(size_t legIdx = 0; legIdx < legs.size(); ++legIdx)
    {
        legs[legIdx].CreateNodes(*pNodeLegs, legIdx);
    }
}

void InsectModule::Notify()
{
    
    if (pNodeShutdown->IsValueChanged())
    {
        std::string cmd = "sudo shutdown"; // Shutdown commmand.
        if (!pNodeShutdown->Get())
        {
            cmd += " -c"; // Cancel the shutdown request.
            std::system(cmd.c_str());
        }
        else
        {
            //cmd += " -P"; // Power of now 
            EnableLegs(false); // Disable all motors.
            std::system(cmd.c_str()); // Shutdown the OS
        }
    }

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
}

void InsectModule::Execute()
{
    for (auto leg : legs)
    {
        //leg.Execute();
        //leg.SetGoal();
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
    for (auto leg : legs)
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
