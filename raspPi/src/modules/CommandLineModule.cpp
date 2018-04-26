#include "CommandLineModule.hpp"
//#include "../cmd.h"

static const char* S_FAIL = "FAIL";
static const char* S_OK = "OK";
#define RETURN_FAIL pNodeCommand->SetInfo(S_FAIL); return;
#define RETURN_OK pNodeCommand->SetInfo(S_OK); return;

void CommandLineModule::Publish()
{
}

void CommandLineModule::CreateNodes(BaseNode& rootNode)
{
    pNodeCommand = rootNode.FindOrCreateChild<StringNode>("Cmd");
    pNodeCommand->SetFlag(FlagType::hide, true);
    // Subscribe to changes.
    pNodeCommand->Subscribe(this);
}

void CommandLineModule::Notify()
{
    if (pNodeCommand->IsValueChanged() && !pNodeCommand->IsAttributeChanged(infoAttrib.GetID()))
    {
        pNodeCommand->SetInfo("");
        ProcessCmd(pNodeCommand->Get().c_str());
    }
}

    // Command line syntax: 
    // Set a value          $ cmdName=<value>
    // Response             $ <OK> or <FAIL>
    //
    // Query a value        $ ?cmdName
    // Response             $ <Value> or <FAIL>
    //
    // Set an attribute     $ cmdName attrName=<attrValue>
    // Response             $ <OK> or <FAIL>
    //
    // query an attribute   $ ?cmdName attrName
    // Response             $ <attrValue> or <FAIL>
    // 
    // query all info about a node:
    // Response             $ ??cmdName 
    //                      $ <FAIL> or multiple lines in the following format:
    //                      $ cmdName <nodeType>
    //                      $ attrName1 = <attrValue>
    //                      $ attrName2 = <attrValue>
    //                      $ ....
    //
    // Query all commands   $ --h, --help
    //
    // Add a new command. (Creates a mirror node to <absolute_node_path> under the command node.) 
    //                      $ ++Add cmdName <absolute_node_path>
    //                      $ <OK> or <FAIL>
    //
    // Remove a command. Deletes a mirror node under the command node.
    //                      $ --delete cmdName
    //                      $ <OK> or <FAIL>
    // 
    // Ideas:
    // Continuously monitor a node:
    //                      $ ->cmdName [t=<timeout_ms>]
    // Response             $ <FAIL> or <Value> continuously sent at an interval of <timeout_ms> 
    //                      $ Sending a new command will end the monitoring state.
    //
    // Note: Create/Destroy nodes could make add/delete command obsolete?
    // Create a new node in the node tree. <nodeType> is the type of the end node. All other nodes are assumed to be of type 'node'.
    //                      $ ++Create nodeName type=<nodeType> path=<absolute_node_path>
    // Response             $ <OK> or <FAIL>
    //
    // Delete a node from the node tree. (node must be an end node/no children) 
    // What if some one is depending on node ?? probably not a good idea to delete nodes?
    //                      $ --Destroy <absolute_node_path>

// pCmd is null terminated.
void CommandLineModule::ProcessCmd(const char* pCmd)
{
    // Find command name

    // Search for special commands (?, ??, ++, --)
    const char* pCurChar = pCmd;
    switch (pCurChar[0])
    {
    case 0:     // Empty command string.
        return; 
    case '?':   // Handle a query command.
    {
        ++pCurChar;
        bool allInfo = pCurChar[0] == '?';
        if (allInfo) ++pCurChar;
        return QueryCmd(pCurChar, allInfo);
    }
    case '@':   // Handle add mirror node to commands.
        ++pCurChar;
        return MirrorNodeCmd(pCurChar);
    case '-':
        ++pCurChar;
        return; // DestroyNodeCmd(pCurChar);
    case ':':
        //return ShellCmd();
    default:    // Handle assignment command.
        return AssignmentCmd(pCurChar);
    }
}
//////////////////////
// QueryCmd - Syntax:
// Query a value        $ ?cmdName
// Response             $ <Value> or <FAIL>
//
// query an attribute   $ ?cmdName attrName
// Response             $ <attrValue> or <FAIL>
//
// query all info about a node:
//                      $ ??cmdName 
// Response             $ <FAIL> or multiple lines in the following format:
//                      $ cmdName <nodeType>
//                      $ attrName1 = <attrValue>
//                      $ attrName2 = <attrValue>
//                      $ ....
void CommandLineModule::QueryCmd(const char* pCmd, bool allInfo)
{
    // Try to lookup node.
    const char* pNodeNameEnd = pCmd;
    BaseNode* pQueryNode = FindNode(&pNodeNameEnd);
    if (pQueryNode == nullptr)
    {
        RETURN_FAIL
    }
    // Node found 
    if (allInfo)
    {
        std::string tmpStr;
        std::string retVal;
        for (attribID_t i = 0; i < Attribute::GetNumAttributes(); ++i)
        {
            if (!pQueryNode->IsAttributeUsed(i)) { continue; }
            if (pQueryNode->GetAttribute(i, tmpStr))
            {
                retVal.append(Attribute::GetAttributeName(i));
                retVal.push_back('=');
                retVal.append(tmpStr.append("\n"));
            }
        }
        pNodeCommand->SetInfo(retVal.c_str());
        // Display all info
        return;
    }
    attribID_t attr = FindAttribute(&pNodeNameEnd);
    if (attr == INVALID_ATTRIBUTE_ID)
    {
        RETURN_FAIL
    }

    std::string sAttribValue;
    if (pQueryNode->GetAttribute(attr, sAttribValue))
    {   // Return the queried attribute.
        pNodeCommand->SetInfo(sAttribValue.c_str());
        return;
    }
    else
    {
        RETURN_FAIL
    }
}

//////////////////////
// AssignmentCmd - Syntax:
// Set a value          $ cmdName=<value>
// Response             $ <OK> or <FAIL>
//
// Set an attribute     $ cmdName attrName=<attrValue>
// Response             $ <OK> or <FAIL>
void CommandLineModule::AssignmentCmd(const char* pCmd)
{
    // Try to lookup node.
    const char* pNodeNameEnd = pCmd;
    BaseNode* pQueryNode = FindNode(&pNodeNameEnd);
    if (pQueryNode == nullptr)
    {
        RETURN_FAIL
    }

    if (*pNodeNameEnd == '=')
    {
        ++pNodeNameEnd;
        // Assigment of default attribute (value)
        if (pQueryNode->SetAttribute(valueAttrib.GetID(), pNodeNameEnd))
        {
            RETURN_OK
        }
        {
            RETURN_FAIL
        }
    }

    attribID_t attr = FindAttribute(&pNodeNameEnd);
    if (attr == INVALID_ATTRIBUTE_ID)
    {
        RETURN_FAIL
    }

    if (*pNodeNameEnd == '=')
    {
        ++pNodeNameEnd;
        if (pQueryNode->SetAttribute(attr, pNodeNameEnd))
        {
            RETURN_OK
        }
        {
            RETURN_FAIL
        }
    }
}

//////////////////////
// MirrorNodeCmd - Syntax:
//                      $ @newNodeName <absolute_node_path>
// Response             $ <OK> or <FAIL>
// Create a new mirror node to the specified absolute_node_path.
// Note the absolute path must not contain mirror nodes.
void CommandLineModule::MirrorNodeCmd(const char* pCmd) 
{
    // Skip leading white spaces.
    const char* pNodeName = SkipLeadingWhitespace(pCmd);
    const char* pNodeNameEnd = FindEndOfName(pNodeName);
    // Check for empty node name.
    if (pNodeName == pNodeNameEnd)
    {
        RETURN_FAIL;
    }
    std::string nodeNode = std::string(pNodeName, pNodeNameEnd);
    const char* pPath = SkipLeadingWhitespace(pNodeNameEnd);
    const char* pPathEnd = FindEndOfName(pPath);
    std::string mirrorPath(pPath, pPathEnd);
    if(pNodeCommand->FindNode(mirrorPath, false, false) == nullptr) // Check that mirror node exist.
    {
        RETURN_FAIL
    }
    // Create the new mirror node and link it.
    MirrorNode* pMirrorNode = pNodeCommand->AddChild<MirrorNode>(nodeNode, mirrorPath);
    if (!pMirrorNode->LinkMirror())
    {
        RETURN_FAIL
    }
    RETURN_OK
}

// Find the attribute - if none is specified, the value attribute is returned.
attribID_t CommandLineModule::FindAttribute(const char **pStr)
{
    // Skip leading white spaces.
    const char* pAttribName = SkipLeadingWhitespace(*pStr);
    const char* pAttribNameEnd = FindEndOfName(pAttribName);
    if (pAttribName != pAttribNameEnd)
    {
        *pStr = SkipLeadingWhitespace(pAttribNameEnd);
        // Get copy of attrib name.
        std::string sAttribName(pAttribName, pAttribNameEnd); // Get attribute name from cmd text.
        return Attribute::GetAttributeID(sAttribName.c_str());
    }
    return valueAttrib.GetID(); // Return default attribute (value)
}

BaseNode* CommandLineModule::FindNode(const char **pStr)
{
    // Skip leading white spaces.
    const char* pNodeName = SkipLeadingWhitespace(*pStr);
    const char* pNodeNameEnd = FindEndOfName(pNodeName);
    // Check for empty node name.
    if (pNodeName == pNodeNameEnd)
    {
        return nullptr;
    }
    *pStr = SkipLeadingWhitespace(pNodeNameEnd);
    // Get copy of node name/path.
    std::string nodePath(pNodeName, pNodeNameEnd); // Get nodepath from cmd text.
    // Try to lookup node.
    return pNodeCommand->FindNode(nodePath, true, true);
}

const char* CommandLineModule::SkipLeadingWhitespace(const char *pStr)
{
    // Skip leading white spaces.
    while (*pStr != 0 && isspace((unsigned char)*pStr))
    {
        ++pStr;
    }
    return pStr;
}

const char* CommandLineModule::FindEndOfName(const char *pStr)
{
    // Skip leading white spaces.
    while (*pStr != 0 && *pStr != '=' && !isspace((unsigned char)*pStr))
    {
        ++pStr;
    }
    return pStr;
}