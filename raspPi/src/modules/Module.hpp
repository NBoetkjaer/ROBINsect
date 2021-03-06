#pragma once
#include "../nodes/Nodes.hpp"

class Module
{
protected:
    static const int NoTimeOut = -1;
public:
    Module(){}
    virtual ~Module() {};
    // CreateNodes is used to create all nodes which is "owened" by this module.
    // Nodes should be created with default values, in order to be able to recover the configuration file,
    // in case it is lost.
    virtual void CreateNodes(BaseNode& rootNode) {};
    // LookupNodes is used to find nodes that is not created by CreateNodes, eg. mirrors.
    // LoookupNodes should not try to set any attributes, since this will overwrite settings loaded
    // from configuration file. 
    virtual void LookupNodes() {};
    virtual void Execute() {}; // May add info into the tree
    virtual void OnTimer() {}; // May add info into the tree
    virtual void Publish() {}; // May not add info to the tree
    inline void SetTimeout(int milliseconds) { timeoutPeriod_ms = milliseconds; };
    inline int GetTimeout() const { return timeoutPeriod_ms;};
    inline  void SetElapsedUs(float usec) { elapsed_us = usec;};
protected:
    int timeoutPeriod_ms = NoTimeOut; // A negative value means timer is disabled.
    float elapsed_us = 0;
};