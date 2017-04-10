#pragma once
#include "../nodes/Nodes.hpp"

class Module
{
public:
    Module():timeoutPeriod_ms(-1){}
    virtual ~Module() {};
    // CreateNodes is used to create all nodes which is "owened" by this module.
    // Nodes should be created with default values, in order to be able to recover the configuration file,
    // in case it is lost.
    virtual void CreateNodes(BaseNode& rootNode) {};
    // LookupNodes is used to find nodes that is not created by CreateNodes, eg. mirrors.
    // LoookupNodes should not try to set any attributes, since this will overwrite settings loaded
    // from configuration file. 
    virtual void LookupNodes() {};
    virtual void Execute() = 0; // May add info into the tree
    virtual void OnTimer() {}; // May add info into the tree
    virtual void Publish() {}; // May not add info to the tree
    inline void SetTimeout(int milliseconds) { timeoutPeriod_ms = milliseconds; };
    inline int GetTimeout() const { return timeoutPeriod_ms; };
private:
    int timeoutPeriod_ms; // A negative value means timer is disabled.
};