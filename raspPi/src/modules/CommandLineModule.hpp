#pragma once

#include "Module.hpp"
//#include <array>
//#include <string>

class CommandLineModule : public Module, public NodeObserver
{
private:
    StringNode* pNodeCommand;
    StringNode* pNodeOutput;
public:
    CommandLineModule(){};
    virtual ~CommandLineModule(){};
    virtual void Publish() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
private:
    void ProcessCmd(const char* pCmd);
    void QueryCmd(const char* pCmd, bool allInfo);
    void AssignmentCmd(const char* pCmd);
    void MirrorNodeCmd(const char* pCmd);
    // Inherited via NodeObserver
    virtual void Notify() override;

    // Utillity functions
    BaseNode* FindNode(const char **pStr);
    attribID_t FindAttribute(const char **pStr);
    const char* SkipLeadingWhitespace(const char *pStr);
    const char* FindEndOfName(const char *pStr);
};