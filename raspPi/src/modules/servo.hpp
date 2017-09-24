#pragma once
#include "../nodes/Nodes.hpp"

class Servo : public NodeObserver
{
    public:
        Servo(); 
        void CreateNodes(BaseNode& parentNode, int servoNumber);
        void LookupNodes();
        unsigned short GetPWM(float angle);
        float GetAngle(unsigned short pwm);

        virtual void Notify() override;
    private:
        BaseNode *pServoNode;
        BoolNode *pNodeEnabled;
        UInt16Node* pNodePWM;
        FloatNode* pNodeAngle;
        FloatNode* pNodeSetAngle;
        BaseNode *pCalibrationNode;
        UInt16Node* pNodeMinPWM;
        UInt16Node* pNodeMaxPWM;
        FloatNode* pNodeMinAngle;
        FloatNode* pNodeMaxAngle;

        float slope, offset;
};
