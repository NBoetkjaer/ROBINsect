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
	//SetRange(200, 650, -90.f, 90.f);
}

void Servo::Init(BaseNode& parentNode, int servoNumber)
{
    string nodeName = std::to_string(servoNumber);
    BaseNode *pServoNode = parentNode.FindOrCreateChild(nodeName);
    if (pServoNode == nullptr)
    {
        // ToDo error.
    }
    pServoNode->Subscribe(this);

    pNodePWM = pServoNode->FindOrCreateChild<UInt16Node>("PWM", 0, 150, 700);
    pNodeAngle = pServoNode->FindOrCreateChild<FloatNode>("Angle");
    pNodeAngle->SetAttribute(unitAttrib.GetID(), "deg");

    pNodeSetAngle = pServoNode->FindOrCreateChild<FloatNode>("SetAngle", 0.0f, -90.0f, 90.0f);
    pNodeSetAngle->SetAttribute(unitAttrib.GetID(), "deg");

    pCalibrationNode = pServoNode->FindOrCreateChild("Calibration");
    if (pCalibrationNode == nullptr)
    {
        // ToDo error.
    }

    pNodeMinPWM = pCalibrationNode->FindOrCreateChild<UInt16Node>("MinPWM", 200);
    pNodeMaxPWM = pCalibrationNode->FindOrCreateChild<UInt16Node>("MaxPWM", 650);
    pNodeMinAngle = pCalibrationNode->FindOrCreateChild<FloatNode>("MinAngle", -90.f);
    pNodeMinAngle->SetAttribute(unitAttrib.GetID(), "deg");
    pNodeMaxAngle = pCalibrationNode->FindOrCreateChild<FloatNode>("MaxAngle", 90.0f);
    pNodeMaxAngle->SetAttribute(unitAttrib.GetID(), "deg");
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
    if (pNodeSetAngle->IsValueChanged())
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