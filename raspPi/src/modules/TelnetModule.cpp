#include "TelnetModule.hpp"
#include <ctype.h>
#include <iostream> // ToDo: Remove all debug print ...

using namespace std;

TelnetModule::TelnetModule()
{
    SetTimeout(200);
}

TelnetModule::~TelnetModule()
{
}

void TelnetModule::CreateNodes(BaseNode& rootNode)
{
    pCurrentNode = &rootNode;
    BaseNode* pNode = rootNode.FindOrCreateChild<BaseNode>("Network");
    pNode = pNode->FindOrCreateChild<BaseNode>("TelnetSocket");
    TcpSocketModule::CreateNodes(*pNode, 1973);
    rootNode.Subscribe(this);
}

void TelnetModule::Notify()
{
    if (pCurrentNode->AnyChanges())
    {
        updateOutput = true;
    }
}

void TelnetModule::OnTimer()
{
    if (updateOutput)
    {
        PrintNodes(clearConsole);
        updateOutput = false;
        clearConsole = false;
    }
}

void TelnetModule::Publish()
{
    if (consoleOutput.size() > 0)
    {
        SendData(consoleOutput.data(), consoleOutput.size());
        consoleOutput.clear();
    }
}

void TelnetModule::DataReceived(char* pData, size_t dataLen)
{
    // remove trailing whitespaces.
    while (dataLen > 0 && isspace((unsigned char)pData[dataLen - 1]))
    {
        dataLen--;
        pData[dataLen] = 0;
    }
    ProcessCmd(pData, dataLen);
    updateOutput = true;
    clearConsole = true;
}

void TelnetModule::ProcessCmd(const char* pCmd, size_t dataLen)
{    
    const char* pChar = pCmd;
    const char* pEndChar = pCmd + dataLen; // One past the last character.
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
    // Skip leading white spaces.
    while (pChar != pEndChar && isspace((unsigned char)*pChar))
    {
        ++pChar;
    }
    const char* pStartNodePath = pChar;
    const char* pStartAttributeName = nullptr;
    bool assignment = false;
    // Read entire NodePath and attribute name if present.
    while (pChar != pEndChar)
    {
        ++pChar;
        if (pChar == pEndChar || isspace((unsigned char)(*pChar)) || *pChar == '=')
        {
            if (pCmdNode == nullptr)
            {
                std::string nodePath(pStartNodePath, pChar); // Get nodepath from cmd text.
                pCmdNode = pCurrentNode->FindNode(nodePath, true, true);
                if (pCmdNode == nullptr) return; // Node not found.
            }
            else if(attrId == INVALID_ATTRIBUTE_ID)
            {
                std::string attribName(pStartAttributeName, pChar); // Get attribute name.
                attrId = Attribute::GetAttributeID(attribName.c_str());
                if (attrId == INVALID_ATTRIBUTE_ID) return; // Unknown attribte. (ToDo should we create ??)
            }
            // Skip leading white spaces.
            while (pChar != pEndChar && isspace((unsigned char)*pChar))
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
    }
}

void TelnetModule::PrintNodes(bool clear)
{
    if (clear)
    {
        consoleOutput = ERASE_ALL HOME FOREGROUND(GREEN);
    }
    else
    {
        consoleOutput = SAVE_POS HOME FOREGROUND(GREEN);
    }
    std::string tmpStr = "/";
    BaseNode * pNode = pCurrentNode;
    while (pNode)
    {
        tmpStr = "/" + pNode->GetName() + tmpStr;
        pNode = pNode->GetParent();
    }
    consoleOutput += "Current node: " + tmpStr + NEWLINE;
    consoleOutput += FOREGROUND(DEFAULT_COLOR);

    bool bShowAll = true;
    for (const auto &child : pCurrentNode->GetChilds())
    {
        bool hiddenChild = child->GetFlag(FlagType::hide);
        if (!bShowAll && hiddenChild)
        {
            continue; 
        }
        if (child->AnyChanges())
        {
            consoleOutput += BACKGROUND(RED); 
        }
        if(child->GetChilds().size())
            consoleOutput += " + " BACKGROUND(DEFAULT_COLOR);
        else
            consoleOutput += " - " BACKGROUND(DEFAULT_COLOR);

        if(hiddenChild)
        {
            consoleOutput += FOREGROUND(YELLOW);
        }
        consoleOutput += child->GetName();
        // Make Value the first attribute.
        if (child->GetAttribute(valueAttrib.GetID(), tmpStr))
        {
            if(child->GetFlag(FlagType::readonly))
                consoleOutput += FOREGROUND(DEFAULT_COLOR);
            else
                consoleOutput += FOREGROUND(CYAN);
            consoleOutput +=  "=" + tmpStr;
            if (child->GetAttribute(prefixAttrib.GetID(), tmpStr))
            {

            }
            if (child->GetAttribute(unitAttrib.GetID(), tmpStr))
            {
                consoleOutput += " " + tmpStr;
            }
        }
        consoleOutput += FOREGROUND(DEFAULT_COLOR);
        for (attribID_t i = 0; i < Attribute::GetNumAttributes(); ++i)
        {
            if (!child->IsAttributeUsed(i)) { continue; }
            if(!bShowAll && (i == valueAttrib.GetID() || i == unitAttrib.GetID())) { continue; }
                
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
    
    if (clear)
    {
        consoleOutput += NEWLINE "~:";
    }
    else
    {
        consoleOutput += RESTORE_POS;
    }
}