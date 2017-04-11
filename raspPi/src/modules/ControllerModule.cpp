#include "ControllerModule.hpp"

using namespace std;
ControllerModule::ControllerModule()
{
}

void ControllerModule::CreateNodes(BaseNode& rootNode)
{
    BaseNode* pNodeHardware = rootNode.FindOrCreateChild("Hardware");
    BaseNode* pNodeControllers = pNodeHardware->FindOrCreateChild("PWMControllers");

    for (size_t iCtrl = 0; iCtrl < controllers.size(); ++iCtrl)
    {
        controllers[iCtrl].CreateNodes(*pNodeControllers, iCtrl);
    }
}
void ControllerModule::LookupNodes()
{
    for (auto& ctrl :controllers)
    {
        ctrl.LookupNodes();
    }
}

void ControllerModule::Publish()
{
    for (auto& ctrl : controllers)
    {
        ctrl.Publish();
    }
}

void ControllerModule::Notify()
{

}



