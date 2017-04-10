#pragma once

#include "Module.hpp"
#include <array>
#include "servo.hpp"

class ServoModule : public Module, public NodeObserver
{
public:
    ServoModule();
    virtual ~ServoModule();

    virtual void Execute() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void LookupNodes() override;

    virtual void Notify() override;
private:
    static const int numServos = 18;
    std::array<Servo, numServos > servos;
};