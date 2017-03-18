#include "NetworkModule.hpp"
#include <ctype.h>
#include <iostream>

using namespace std;

NetworkModule::NetworkModule():
    state(State::Initialize)
{

}

NetworkModule::~NetworkModule()
{

}

void NetworkModule::Init(BaseNode& rootNode)
{
    pCurrentNode = &rootNode;
    BaseNode* pNode = rootNode.AddChild<BaseNode>("TelnetSocket");
    pRcvNode = pNode->AddChild<Int64Node>("BytesRecived", 0);
    pSentNode = pNode->AddChild<Int64Node>("BytesSent", 0);
    pConnectedNode = pNode->AddChild<BoolNode>("Connected", false);

    rootNode.Subscribe(this);
}

void NetworkModule::Notify()
{
    if (pCurrentNode->AnyRecentChanges())
    {
        PrintNodes();
    }
}

void NetworkModule::Execute()
{
    switch (state)
    {
    case State::Initialize:
        std::cout << "Discover Bind " << sockDiscover.Bind(1974, SocketType::UDP) << endl;
        std::cout << "Listen Bind " << sockListen.Bind(1973, SocketType::TCP) << endl;
        std::cout << "Listen " << sockListen.Listen() << endl;
        state = State::Listning;
        pConnectedNode->Set(false);
        return;
    case State::Listning:
        if(sockListen.IsReadPending(0))
        {
            std::cout << "Accept " << sockAccept.Accept(sockListen) << endl;
            std::cout << "Shutdown (listen) " << sockListen.Shutdown() << endl;
            std::cout << "Close (listen)" << sockListen.Close() << endl;
            state = State::Connected;
            sockAccept.SetBlocking(false);
            pConnectedNode->Set(true);
        }
        break;
    case State::Connected:
        if(sockAccept.IsReadPending(0))
        {
            size_t dataLen = buffer.size(); 
            int retVal = sockAccept.Recieve(buffer.data(), &dataLen);
            std::cout << "Recieve " << retVal << ": ";
            if(dataLen == 0)
            {
                // Remote end is shutdown
                std::cout << "Remote end disconnected - Shutdown (sockAccept) " << sockAccept.Shutdown() << endl;
                std::cout << "Close socket" << sockAccept.Close() << endl;
                sockDiscover.Close();
                state = State::Initialize;
                return;
            }
            buffer.at(dataLen) = 0; // Terminate the string.
            ProcessCmd(buffer.data());
            std::cout << buffer.data() << endl;
            pRcvNode->Set(sockAccept.GetBytesRecieved());
            //PrintNodes();
            // ToDo - Check dataLen and send remaining data.
            dataLen = consoleOutput.size();
            retVal = sockAccept.Send(consoleOutput.data(), &dataLen);
            std::cout << "Send " << retVal << ": ";
            pSentNode->Set(sockAccept.GetBytesSent());
        }
        break;
    }

    if(sockDiscover.IsReadPending(0))
    {   // Send reply to the broadcast.
    
    }
}

void NetworkModule::ProcessCmd(const char* pCmd)
{    
    const char* pChar = pCmd;
    // Command syntax:
    // NodePath [AttributeName] [=value] 
    // Set node value:
    //  NodeName =value
    // Set node attribute value:
    //  NodeName AttribureName=value
    // Change current node:
    //  pathToNewNode (eg. cd ../parentNode/sibling)
    //  partial match should be allowed as long as names are not ambiguous. (eg. ../paren/sib ). 

    BaseNode* pCmdNode = nullptr;
    attribID_t attrId = INVALID_ATTRIBUTE_ID;
    // Skip trailing white spaces.
    while (*pChar != 0 && isspace(*pChar))
    {
        ++pChar;
    }
    const char* pStartNodePath = pChar;
    const char* pStartAttributeName = nullptr;
    bool assignment = false;
    // Read entire NodePath and attribute name if present.
    while (*pChar != 0)
    {
        ++pChar;
        if (*pChar == 0 || isspace(unsigned char(*pChar)) || *pChar == '=')
        {
            if (pCmdNode == nullptr)
            {
                std::string nodePath(pStartNodePath, pChar); // Get nodepath from cmd text.
                pCmdNode = pCurrentNode->FindNode(nodePath, true);
                if (pCmdNode == nullptr) return; // Node not found.
            }
            else if(attrId == INVALID_ATTRIBUTE_ID)
            {
                std::string attribName(pStartAttributeName, pChar); // Get attribute name.
                attrId = Attribute::GetAttributeID(attribName.c_str());
                if (attrId == INVALID_ATTRIBUTE_ID) return; // Unknown attribte. (ToDo should we create ??)
            }
            // Skip trailing white spaces.
            while (*pChar != 0 && isspace(*pChar))
            {
                ++pChar;
            }
            pStartAttributeName = pChar;
            if (*pChar == '=')
            {
                pChar++;
                assignment = true;
                break;
            }
            else if (pCmdNode != nullptr && attrId != INVALID_ATTRIBUTE_ID)
            {
                return; // Error! Invalid syntax. Expected assignment
            }
        }
    }
    if (pCmdNode != nullptr)
    {
        if (assignment)
        {
            if (attrId != INVALID_ATTRIBUTE_ID)
            {
                pCmdNode->SetAttribute(attrId, pChar);
            }
            else // Default to value attribute.
            {
                pCmdNode->SetAttribute(valueAttrib.GetID(), pChar);
            }
        }
        else
        {
            pCurrentNode = pCmdNode;
        }
        PrintNodes();
    }
}

void NetworkModule::PrintNodes()
{
    consoleOutput = ERASE_ALL HOME FOREGROUND(GREEN);

    std::string tmpStr = "/";
    BaseNode * pNode = pCurrentNode;
    while (pNode)
    {
        tmpStr = "/" + pNode->GetName() + tmpStr;
        pNode = pNode->GetParent();
    }
    consoleOutput += "Current node: " + tmpStr + NEWLINE;
    consoleOutput += FOREGROUND(DEFAULT_COLOR);

    for (const auto &child : pCurrentNode->GetChilds())
    {
        if(child->AnyRecentChanges()) consoleOutput += BACKGROUND(DEFAULT_COLOR);
        consoleOutput += " - " BACKGROUND(DEFAULT_COLOR);
        consoleOutput += child->GetName();
        // Make Value the first attribute.
        if (child->GetAttribute(valueAttrib.GetID(), tmpStr))
        {
            consoleOutput += FOREGROUND(CYAN);
            consoleOutput +=  "=" + tmpStr;
            if (child->GetAttribute(prefixAttrib.GetID(), tmpStr))
            {

            }
            if (child->GetAttribute(unitAttrib.GetID(), tmpStr))
            {
                consoleOutput += " " + tmpStr;
            }
            consoleOutput += FOREGROUND(DEFAULT_COLOR);
        }

        for (attribID_t i = 0; i < Attribute::GetNumAttributes(); ++i)
        {
            if (!child->IsAttributeUsed(i) || i == valueAttrib.GetID() || i == unitAttrib.GetID())
                continue;
            if (child->GetAttribute(i, tmpStr))
            {
                consoleOutput += " " + Attribute::GetAttributeName(i) + "=" + FOREGROUND(CYAN) + tmpStr + FOREGROUND(DEFAULT_COLOR);
            }
            else
            {
                //assert(false);
            }
        }
        consoleOutput += NEWLINE;
    }
    consoleOutput += ":";
}