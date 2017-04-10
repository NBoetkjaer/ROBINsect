#include "InsectModule.hpp"
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
    //pNodeInsect->Subscribe(this);

    pNodeLegs = pNodeInsect->FindOrCreateChild<BaseNode>("Legs");

    pNodePosition = pNodeInsect->FindOrCreateChild<Pos3D_32f_Node>("BodyPosition");
    for(size_t legIdx = 0; legIdx < legs.size(); ++legIdx)
    {
        legs[legIdx].CreateNodes(*pNodeLegs, legIdx);
    }
}

void InsectModule::Notify()
{
}

void InsectModule::Execute()
{
    for (auto leg : legs)
    {
        leg.Execute();
    }
}