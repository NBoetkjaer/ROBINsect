#pragma once

#include "Module.hpp"
#include "TcpSocketModule.hpp"
#include "../nodes/xml/nodeXmlConverter.hpp"
#include <array>
#include <string>

class XmlViewModule : public TcpSocketModule, public NodeObserver
{
private:
    BaseNode* pViewNode;
public:
    XmlViewModule(const std::string& viewPath);
    virtual ~XmlViewModule() {};
    virtual void Publish() override;
    virtual void CreateNodes(BaseNode& rootNode) override;
    virtual void LookupNodes() override;
private:
    virtual void Connected() override;
    virtual void DataReceived(const char* pData, size_t dataLen) override;
    std::string viewPath;
    bool update;
    bool onlyChanges;
    NodeXmlConverter xmlConverter;
    // Inherited via NodeObserver
    virtual void Notify() override;
};