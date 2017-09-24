#include "controller.hpp"
#include <string>

using namespace std;
Controller::Controller():
    pNodeController(nullptr)
{
}

void Controller::CreateNodes(BaseNode& parentNode, int controllerID)
{
    string nodeName ="device" + std::to_string(controllerID);
    pNodeController = parentNode.FindOrCreateChild(nodeName);
    //pNodeController->Subscribe(this);
    
    pNodeI2CDevice = pNodeController->FindOrCreateChild<UInt16Node>("i2c_device", 1);
    pNodeI2CAddr = pNodeController->FindOrCreateChild<UInt32Node>("i2c_Addr", 0x40 + controllerID);
    pNodePwmFrequency = pNodeController->FindOrCreateChild<FloatNode>("pwmFrequency", 60.0f, 50.0f, 200.0f );
    pNodeController->FindOrCreateChild("Channels");
    for (size_t iChan = 0; iChan < channels.size(); ++iChan)
    {
        nodeName = "ch" + to_string(iChan);
        channels[iChan] = pNodeController->FindOrCreateChild<UInt16Node>(nodeName, 0, 0, 800);
    }
}

void Controller::LookupNodes()
{
    // ToDo handle any errors and use values from node tree.
    if (!controller.init(pNodeI2CDevice->Get(), pNodeI2CAddr->Get()))
        pNodeController->SetInfo("Error init");

    if(!controller.setUpdateFrequency(pNodePwmFrequency->Get()))
        pNodeController->FindOrCreateChild("Error setUpdateFrequency");
}

void Controller::Publish()
{
    bool changes = false;
    for (size_t iChan = 0; iChan < channels.size(); ++iChan)
    {
        if (channels[iChan]->IsValueChanged())
        {
            changes = true;
            controller.setPWM((__u8)iChan, 0, channels[iChan]->Get());
        }
    }
    if (changes)
    {
        // ToDo is this necessary?
        //controller.selectAddress(pNodeI2CAddr->Get());
        controller.writeAllChannels();
    }
}

void Controller::Notify()
{
}