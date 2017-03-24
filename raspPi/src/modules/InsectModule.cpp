#include "InsectModule.hpp"
using namespace std;


InsectModule::InsectModule()
{
}

InsectModule::~InsectModule()
{
}

void InsectModule::Init(BaseNode& rootNode)
{
    pNodeInsect = rootNode.AddChild<BaseNode>("Insect");
    pNodeInsect->Subscribe(this);

    pNodePosition = pNodeInsect->AddChild<Pos3D_32f_Node>("BodyPosition");
    for(int legIdx = 0; legIdx < numLegs; ++legIdx) 
    {
        legs[legIdx].Init(*pNodeInsect, legIdx);
    }
}

void InsectModule::Notify()
{
}

void InsectModule::Execute()
{
}