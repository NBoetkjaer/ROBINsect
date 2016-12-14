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

bool BaseNode::SetAttribute(attribID_t attribID, const char* pAttributeValue)
{

    if(flags.GetID() == attribID)
    {
        return true;
    }
    else if(info.GetID() == attribID){

    }
    //else if(info.GetID() == attribID){

    return false;
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
// An absolute path is specified with a trailing "/" eg. "/node1/node2/nodeToFind", in which case the function will begin the search from the root node.
// The syntax of a relative path is "node2/nodeToFind" and the search is started from this node.
// Return value: If the node is found a pointer to the requested node is returned, otherwise a nullptr is returned.
BaseNode* BaseNode::FindNode(std::string nodePath)
{
    BaseNode* pNode = this;
    size_t pos = 0;
    const char pathDelimiter = '/';
    if (nodePath.front() == pathDelimiter)
    {
        // Traverse to root.
        while (pNode->pParent)
        {
            pNode = pNode->pParent;
        }
        pos = 1;
    }
    // Begin search
    size_t tmpPos = nodePath.find(pathDelimiter, pos);
    std::string nodeName;
    if (tmpPos != std::string::npos)
    {
        nodeName = nodePath.substr(pos, tmpPos - pos);
        pos = tmpPos;
        for (const auto &child : pNode->children)
        {
            if (nodeName == child->name)
            {
                pNode = child.get();
                break;
            }
        }
    }
    return nullptr;
}