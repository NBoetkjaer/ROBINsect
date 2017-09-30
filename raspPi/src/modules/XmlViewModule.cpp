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
        static const std::string transferStart = "<" + xmlConverter.Transaction + ">\n";
        static const std::string transferEnd = "</" + xmlConverter.Transaction + ">\n";
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

void XmlViewModule::DataReceived(char* pData, size_t dataLen)
{
    const std::string transferStart = "<" + xmlConverter.Transaction;
    const std::string transferEnd = "</" + xmlConverter.Transaction + ">";

    if (pViewNode == nullptr) return;
    size_t currPos = transactionReceiveBuffer.size();
    transactionReceiveBuffer.append(pData, dataLen);
    size_t posStart, posEnd;
    while (true) // ??
    {
        // No current transfer.
        if (transactionStartPos < 0)
        {
            // Look for start of a new transaction.
            posStart = std::max(0, (int)currPos - (int)transferStart.size());
            posStart = transactionReceiveBuffer.find(transferStart, posStart);
            if (posStart != std::string::npos)
            {

                // Found start now look for end of a transaction.
                posEnd = transactionReceiveBuffer.find(transferEnd, posStart);
                if (posEnd != std::string::npos)
                {   // Found entire transaction.
                    // Move current position forward past transfer end.
                    currPos = posEnd + transferEnd.size();
                    // Handle message.
                    strTransaction = transactionReceiveBuffer.substr(posStart, currPos - posStart);
                    xmlConverter.UpdateTreeFromXml(pViewNode, strTransaction);
                    // Remove old message from recieve buffer.
                    transactionReceiveBuffer = transactionReceiveBuffer.substr(currPos);
                    currPos = 0;
                }
                else
                {   // Save start position of the transaction.
                    transactionStartPos = posStart;
                    return;
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            // Look for end of a transaction.
            posEnd = std::max(0, (int)currPos - (int)transferEnd.size());
            posEnd = transactionReceiveBuffer.find(transferEnd, posEnd);
            if (posEnd != std::string::npos)
            {
                // Move current position forward past transfer end.
                currPos = posEnd + transferEnd.size();
                // Handle message end
                strTransaction = transactionReceiveBuffer.substr(transactionStartPos, currPos - transactionStartPos);
                xmlConverter.UpdateTreeFromXml(pViewNode, strTransaction);
                transactionStartPos = -1;
                // Remove old message from recieve buffer.
                transactionReceiveBuffer = transactionReceiveBuffer.substr(currPos);
                currPos = 0;
            }
            else
            {
                return;
            }
        }
    }
}