#pragma once
#include "../nodes/Nodes.hpp"

#include <array>
#include "hardware/pca9685.hpp"


class Controller : public NodeObserver
{
    public:
        Controller();
        void CreateNodes(BaseNode& parentNode, int controllerID);
        void LookupNodes();
        virtual void Notify() override;
        void Publish();
    private:
        pca9685 controller;
        std::array<UInt16Node*, 16> channels;

        BaseNode *pNodeController;
        
};
