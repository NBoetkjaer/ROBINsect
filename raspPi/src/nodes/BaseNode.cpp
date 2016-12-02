#include "BaseNode.hpp"

#include <algorithm>
using namespace std;


BaseNode::BaseNode(const std::string &nodeName, BaseNode* pParentNode, NodeType type)
    :name(nodeName), nodeType(type), pParent(pParentNode)
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

BaseNode* BaseNode::FindNode(std::string nodePath)
{
    // ToDo implement.
    return nullptr;
}