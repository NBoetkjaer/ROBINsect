#pragma once

#include "Module.hpp"
#include <array>
#include "controller.hpp"

class ControllerModule : public Module, public NodeObserver
{
public:
    ControllerModule();
    virtual ~ControllerModule() {};
    //virtual void Execute() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void LookupNodes() override;
    virtual void Publish() override;
    virtual void Notify() override;
private:
    static const int ctrlCount = 2;
    std::array<Controller, ctrlCount> controllers;
};
