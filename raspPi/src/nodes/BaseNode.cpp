#include "BaseNode.hpp"

#include <algorithm>

using namespace std;
using namespace Util;

// Register attributes.
static Attribute flagsAttrib("flags");
static Attribute typeAttrib("type");
static Attribute infoAttrib("info");

Attribute valueAttrib("value");
Attribute rangeAttrib("range");
// Define flag names 
std::array< std::tuple<const string, FlagType>, (size_t)FlagType::numflags > BaseNode::flagNames =
{
    make_tuple("hide"       , FlagType::hide),
    make_tuple("readonly"   , FlagType::readonly),
    make_tuple("logging"    , FlagType::logging),
    make_tuple("persist"    , FlagType::persist),
    make_tuple("query"      , FlagType::query),
};


BaseNode::BaseNode(const std::string &nodeName, BaseNode* pParentNode):
    isChanged(true),
	changes(0),
    name(nodeName),
    pParent(pParentNode)
{
}

BaseNode::~BaseNode()
{
}

// Observer 
void BaseNode::Subscribe(NodeObserver* pObserver)
{
    // Check if the same observer is already subscribing.
    if ( std::find(subscribers.begin(), subscribers.end(), pObserver) != subscribers.end() )
        return;
    // Add observer to list of subscribers.
    subscribers.push_back(pObserver);

}

void BaseNode::UnSubscribe(NodeObserver* pObserver)
{
    subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(),
        [&](NodeObserver* pObs) {return pObs == pObserver;}), subscribers.end() );
}

void BaseNode::Notify()
{
    for(auto pObserver : subscribers)
    {
        pObserver->Notify();
    }
}
// Flag attribute
void  BaseNode::SetFlags(const char* pValues)
{
    // Syntax: "[+/-]flag1|[+/-]flag2" eg. "+flag1|+flag2|-flag3" set 
    // Parse string and set flags accordingly.
    static const char flagDelimiter = '|';
    bool moreflags = true;
    FlagType newFlags = (FlagType) 0;


    if (pValues == nullptr) return;
    while (moreflags)
    {
        FlagType flag = FlagType::invalidFlag;
        // End of string stop parsing.
        if (*pValues == 0) break;

        bool removeFlag = false;
        if (pValues[0] == '-')
        {
            removeFlag = true;
            pValues++;
        }
        else if (pValues[0] == '+')
        {
            pValues++;
        }

        // Iterate over all flags to find a string match.    
        for (const auto &flagTuple : flagNames)
        {
            const char* pFlagName = get<0>(flagTuple).c_str();
            const char* pValue = pValues;

            while (*pValue == *pFlagName  && *pFlagName != 0 && *pValue != 0 && *pValue != flagDelimiter)
            {
                pValue++;
                pFlagName++;
            }

            // did we match to the end of flag name?
            if (*pFlagName == 0)
            {
                if (*pValue == flagDelimiter || *pValue == 0)
                {                    
                    if (*pValue == flagDelimiter) pValues = pValue + 1;
					else moreflags = false;
					flag = get<1>(flagTuple);
                    break;
                }
            }
        }
        if (flag != FlagType::invalidFlag)
        {
            SetFlag(flag, !removeFlag);
        }
    } // End of while loop.
}

void BaseNode::SetFlag(FlagType flag, bool value)
{
    FlagType oldFlags = nodeFlag;
    if (value)
    {
        // Add flag.
        nodeFlag |= (FlagType)(1 << (uint32_t)flag);
    }
    else
    {
        // Remove flag
        nodeFlag &= (FlagType)(~(1 << (uint32_t)flag));
    }
    if (oldFlags != nodeFlag)
    {
        SetAttributeChanged(flagsAttrib.GetID()); // Mark the change.;
    }
}

bool BaseNode::GetFlag(FlagType flag)
{
    return ((1<<(uint32_t)flag) & (uint32_t)nodeFlag) != 0;
}
// Info attribute
void BaseNode::SetInfo(const char* pValue)
{
	info = pValue;
	SetAttributeChanged(flagsAttrib.GetID()); // Mark the change.
}

// SetAttribute
// Must return true if a given attribute is handled.
bool BaseNode::SetAttribute(attribID_t attribID, const char* pAttributeValue)
{
    if(flagsAttrib.GetID() == attribID)
    {
        SetFlags(pAttributeValue);
    }
    else if(infoAttrib.GetID() == attribID)
	{
		SetInfo(pAttributeValue);
    }
	else 
	{
		return false; // Unknown attribute return false.
	}

    return true;
    // Find attribute index.
    // If found
    //  Set the attribute
    // else
    //  CreateAttribute
    // Indicate attribute changed-
}

void BaseNode::SetValueChanged()
{
    if(!isChanged)
    {
        isChanged = true;
        if(pParent!= nullptr)
        {
            pParent->SetValueChanged();
        }
    }
}

void BaseNode::Print(int indentLevel) const
{
    std::cout << std::string(indentLevel, '-') << name << "  " << typeid(*this).name() << std::endl;
    for(const auto &child : children)
    {
        child->Print(indentLevel + 1);
    }
}

// The argument nodePath contains the relative or absolute path to the node to search for.
// An absolute path is specified with a leading "/" eg. "/node1/node2/nodeToFind", in which case the function will begin the search from the root node.
// The syntax of a relative path is "node2/nodeToFind" or "../node1/node2/nodeToFind" and the search is started from this node.
// Return value: If the node is found a pointer to the requested node is returned, otherwise a nullptr is returned.
BaseNode* BaseNode::FindNode(const std::string& nodePath)
{
    return FindNodeInternal(nodePath.c_str());
}

BaseNode* BaseNode::FindNode(const char * pNodePath)
{
    return FindNodeInternal(pNodePath);
}

BaseNode* BaseNode::FindNodeInternal(const char * pNodePath)
{
    static const char pathDelimiter = '/';
    if (pNodePath == nullptr) return nullptr;
    // Special case for an empty string - just return this.
    if (pNodePath[0] == 0) return this;
    // Special case if first character is a pathDelimiter we should start the search from the root node.
    if (pNodePath[0] == pathDelimiter)
    {
        BaseNode* pNode = this;
        // Traverse to root.
        while (pNode->pParent)
        {
            pNode = pNode->pParent;
        }
        return pNode->FindNodeInternal(pNodePath + 1);
    }
    // Special case if first two characters are ".." - then return the parent if not null.
    if (pNodePath[0] == '.' && pNodePath[1] != 0 && pNodePath[1] == '.' && pParent)
    {
        if (pNodePath[2] == pathDelimiter) return pParent->FindNodeInternal(pNodePath + 3);
        if (pNodePath[2] == 0) return pParent;
    }

    // Otherwise look up (next) node name in nodePath.
    // Otherwise search children for at string match with next node.
    for (const auto &child : children)
    {
        const char* pChildName = child->name.c_str();
        const char* pNodeName = pNodePath;

        while (*pNodeName == *pChildName && *pChildName != 0 && *pNodeName != 0 && *pNodeName != pathDelimiter)
        {
            pNodeName++;
            pChildName++;
        }
        // did we match to the end of child name?
        if (*pChildName == 0)
        {
            if (*pNodeName == pathDelimiter) return child->FindNodeInternal(pNodeName + 1);
            if (*pNodeName == 0) return child.get();
        }
    }
    return nullptr;
}
