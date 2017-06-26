#include "XmlViewModule.hpp"
#include <ctype.h>
#include <iostream> // ToDo: Remove all debug print ...

using namespace std;

XmlViewModule::XmlViewModule(const std::string& path):
    update(false),
    onlyChanges(false),
    viewPath(path),
    pViewNode(nullptr)
{
    xmlConverter.SetResolveMirrors(true);
    xmlConverter.SetXMLIndenting(false);
}

void XmlViewModule::CreateNodes(BaseNode& rootNode)
{
    pViewNode = &rootNode;
    BaseNode* pNode = rootNode.FindOrCreateChild<BaseNode>("Network");
    pNode = pNode->FindOrCreateChild<BaseNode>("ViewSocket");
    TcpSocketModule::CreateNodes(*pNode, 1975);
}

void XmlViewModule::LookupNodes()
{
    pViewNode = pViewNode->FindNode(viewPath.c_str());
    if (pViewNode != nullptr)
    {
        pViewNode->Subscribe(this);
    }
}

void XmlViewModule::Notify()
{
    update = true;
}

void XmlViewModule::Publish()
{
    if (update && pViewNode && IsConnected())
    {
        std::string xmlStr;
        xmlConverter.ConvertToXml(pViewNode, xmlStr, FlagType::none, onlyChanges);
        static const std::string transferStart = "<transaction>\n";
        static const std::string transferEnd = "</transaction>\n";
        SendData(transferStart.data(), transferStart.size());
        SendData(xmlStr.data(), xmlStr.size());
        SendData(transferEnd.data(), transferEnd.size());
    }
    onlyChanges = true;
    update = false;
}

void XmlViewModule::Connected()
{
    onlyChanges = false; // Send full viewnode tree on connection.
    update = true;
}

void XmlViewModule::DataReceived(const char* pData, size_t dataLen)
{
}