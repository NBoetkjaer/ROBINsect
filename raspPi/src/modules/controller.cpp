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
            // Offset is used to spread out the servo pulses, in order to balance the load on the power supply.
            // Servo PWM value is using less than 10 bit, since we have 12 bit available we distribute the pulses 
            // in full 12 bit range.
            uint16_t channelOffset = static_cast<uint16_t>(iChan * (0xFFF - 0x3FF)/16); 
            changes = true;
            controller.setPWM((__u8)iChan, channelOffset , channels[iChan]->Get() + channelOffset);
        }
    }
    if (changes)
    {
        controller.writeAllChannels();
    }
}

void Controller::Notify()
{
}