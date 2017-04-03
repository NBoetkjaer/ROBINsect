#pragma once
#include "../nodes/Nodes.hpp"

class Module
{
public:
    Module():timeoutPeriod_ms(-1){}
    virtual ~Module() {};
    virtual void Init(BaseNode& rootNode) {};
    virtual void Execute() = 0; // May add info into the tree
    virtual void OnTimer() {}; // May add info into the tree
    virtual void Publish() {}; // May not add info to the tree
    inline void SetTimeout(int milliseconds) { timeoutPeriod_ms = milliseconds; };
    inline int GetTimeout() const { return timeoutPeriod_ms; };
private:
    int timeoutPeriod_ms; // A negative value means timer is disabled.
};