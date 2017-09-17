#include "servo.hpp"
#include <string>
using namespace std;
Servo::Servo():
    pNodePWM(nullptr),
    pNodeAngle(nullptr),
    pNodeSetAngle(nullptr),
    pCalibrationNode(nullptr),
    pNodeMinPWM(nullptr),
    pNodeMaxPWM(nullptr),
    pNodeMinAngle(nullptr),
    pNodeMaxAngle(nullptr),
    slope(1.0f),
    offset(0.0f)
{
}

void Servo::CreateNodes(BaseNode& parentNode, int servoNumber)
{
    string nodeName = "s" + std::to_string(servoNumber);
    pServoNode = parentNode.FindOrCreateChild(nodeName);
    if (pServoNode == nullptr)
    {
        // ToDo error.
    }
    pServoNode->Subscribe(this);

    pNodeAngle = pServoNode->FindOrCreateChild<FloatNode>("Angle");
    pNodeAngle->SetAttribute(unitAttrib.GetID(), "deg");
    // Create mirror 
    const int numJoints = 3;
    int legID = servoNumber / numJoints;
    int jointID = servoNumber % numJoints;
    string path = "/Insect/Legs/l" + to_string(legID) + "/Joints/j" + to_string(jointID) + "/jointAngle";
    pServoNode->FindOrCreateChild<MirrorNode>("SetAngle", path);
    int controllerID = servoNumber / 9;
    const int channelGroupSize = 4;
    int channelNo = (legID % 3) * channelGroupSize + jointID;
    path = "/Hardware/PWMControllers/device" + to_string(controllerID) + "/ch" + to_string(channelNo);
    pServoNode->FindOrCreateChild<MirrorNode>("PWM", path);

    pCalibrationNode = pServoNode->FindOrCreateChild("Calibration");
    if (pCalibrationNode == nullptr)
    {
        // ToDo error.
    }

    pNodeMinPWM = pCalibrationNode->FindOrCreateChild<UInt16Node>("MinPWM", 165);
    pNodeMaxPWM = pCalibrationNode->FindOrCreateChild<UInt16Node>("MaxPWM", 580);

    pNodeMinAngle = pCalibrationNode->FindOrCreateChild<FloatNode>("MinAngle", -90.f);
    pNodeMinAngle->SetAttribute(unitAttrib.GetID(), "deg");
    pNodeMaxAngle = pCalibrationNode->FindOrCreateChild<FloatNode>("MaxAngle", 90.0f);
    pNodeMaxAngle->SetAttribute(unitAttrib.GetID(), "deg");
    pCalibrationNode->SetFlag(FlagType::persist, true, true);
}

void Servo::LookupNodes()
{
    pNodeSetAngle = pServoNode->FindNode<FloatNode>("SetAngle");
    pNodePWM = pServoNode->FindNode<UInt16Node>("PWM");
}

void Servo::Notify()
{
    if (pCalibrationNode->AnyChanges())
    {
        float diffAngle = pNodeMaxAngle->Get() - pNodeMinAngle->Get();
        float diffPWM = (float)pNodeMaxPWM->Get() - (float)pNodeMinPWM->Get();
        if (diffAngle != 0.0f)
        {
            slope = diffPWM / diffAngle;
        }

        if (pNodeMaxAngle->Get() > pNodeMinAngle->Get())
        {
            offset = pNodeMinPWM->Get() - pNodeMinAngle->Get() * slope;
        }
        else
        {
            offset = pNodeMaxPWM->Get() - pNodeMaxAngle->Get() * slope;
        }
    }
    if (pNodeSetAngle && pNodeSetAngle->IsValueChanged())
    {
        pNodePWM->Set(GetPWM(pNodeSetAngle->Get()));
        pNodeAngle->Set(GetAngle(pNodePWM->Get()));
    }
}

unsigned short Servo::GetPWM(float angle)
{
	float pwm = angle * slope + offset;
	return (unsigned short)(pwm + 0.5f);
}

float Servo::GetAngle(unsigned short pwm)
{
	return (pwm - offset) / slope;
}