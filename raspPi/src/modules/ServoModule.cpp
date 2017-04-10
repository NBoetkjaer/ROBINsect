#include "ServoModule.hpp"
using namespace std;

ServoModule::ServoModule()
{
}

ServoModule::~ServoModule()
{
}

void ServoModule::Execute()
{
}

void ServoModule::CreateNodes(BaseNode& rootNode)
{
    BaseNode* pNodeHardware = rootNode.FindOrCreateChild("Hardware");
    BaseNode* pNodeServos = pNodeHardware->FindOrCreateChild("Servos");
    for (size_t i = 0; i < servos.size(); ++i)
    {
        servos[i].CreateNodes(*pNodeServos, i);
    }
}

void ServoModule::LookupNodes()
{
    for (size_t i = 0; i < servos.size(); ++i)
    {
        servos[i].LookupNodes();
    }
}

void ServoModule::Notify()
{
}


