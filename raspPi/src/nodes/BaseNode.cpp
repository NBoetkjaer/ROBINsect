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

void BaseNode::Print() const
{
    std::cout << name <<"  " << typeid(*this).name() << std::endl;
    for(const auto &child : children)
    {
        child->Print();
    }
}

BaseNode* BaseNode::FindNode(std::string nodePath)
{
    // ToDo implement.
    return nullptr;
}