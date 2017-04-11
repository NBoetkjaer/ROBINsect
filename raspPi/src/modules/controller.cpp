#include "controller.hpp"
#include <string>

using namespace std;
Controller::Controller():
    pNodeController(nullptr)
{
}

void Controller::CreateNodes(BaseNode& parentNode, int controllerID)
{
    string nodeName = std::to_string(controllerID);
    pNodeController = parentNode.FindOrCreateChild(nodeName);
    pNodeController->Subscribe(this);
    
    pNodeController->FindOrCreateChild<Int32Node>("i2c_Addr", 0x70 + controllerID);
    pNodeController->FindOrCreateChild("Channels");
    for (size_t iChan = 0; iChan < channels.size(); ++iChan)
    {
        nodeName = to_string(iChan);
        channels[iChan] = pNodeController->FindOrCreateChild<UInt16Node>(nodeName, 0, 150, 700);
    }
}

void Controller::LookupNodes()
{
    controller.init(1, 0x70);
}

void Controller::Publish()
{

}

void Controller::Notify()
{
}