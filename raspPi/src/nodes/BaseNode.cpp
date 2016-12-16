#include "BaseNode.hpp"

#include <algorithm>
using namespace std;


// Register attributes.
static Attribute flags("flags");
static Attribute info("info");


BaseNode::BaseNode(const std::string &nodeName, BaseNode* pParentNode, NodeType type):
    isChanged(true),
    name(nodeName),
    nodeType(type),
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
bool BaseNode::SetFlags(const char* pValues)
{
    // Syntax: "[+/-]flag1|[+/-]flag2" eg. "+flag1|+flag2|-flag3" set 
    // Parse string and set flags accordingly.   
    return true;
}

bool BaseNode::SetAttribute(attribID_t attribID, const char* pAttributeValue)
{

    bool retVal = false;
    if(flags.GetID() == attribID)
    {
        return SetFlags(pAttributeValue);
    }
    else if(info.GetID() == attribID){

    }
    //else if(info.GetID() == attribID){


    if (retVal)
    {
        changes |= 1 << attribID; // If change is handled, we mark the change.
    }
    return retVal;
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

    return FindNodeInternal(nodePath, 0);
}

BaseNode* BaseNode::FindNodeInternal(const std::string& nodePath, size_t pos)
{
    static const char pathDelimiter = '/';    

    // Special case for an empty string - just return this, also terminates the recursion.
    if (pos >= nodePath.size()) return this;

    // 
    if (pos == 0)
    {
        // Special case if first character is a pathDelimiter we should start the search from the root node.
        if (nodePath.front() == pathDelimiter)
        {
            BaseNode* pNode = this;
            // Traverse to root.
            while (pNode->pParent)
            {
                pNode = pNode->pParent;
            }
            return pNode->FindNodeInternal(nodePath, pos + 1);
        }
        // Special case if first two characters are ".." - then return the parent if not null.
        if (nodePath.size() == 2 && nodePath == ".." && pParent)
        {
            return pParent;
        }
    }

    // Otherwise look up (next) node name in nodePath.
    size_t tmpPos = nodePath.find(pathDelimiter, pos);
    size_t len = tmpPos == std::string::npos ? nodePath.size() - pos : tmpPos - pos;
    std::string nodeName = nodePath.substr(pos, len);

    // Special case "../" characters means select parent.  
    // ??? what about "./" ?? should we handle this ???
    if (nodeName == ".." && pParent != nullptr)
    {
        return pParent->FindNodeInternal(nodePath, pos + len + 1);
    }

    // Find the node name amongst children.
    for (const auto &child : children)
    {
        if (nodeName == child->name)
        {
            BaseNode* pNode = child.get();
            return pNode->FindNodeInternal(nodePath, pos + len + 1);;
        }
    }

    // No match return nullptr;
    return nullptr;
}