#include "DiscoveryModule.hpp"

using namespace std;

DiscoveryModule::DiscoveryModule():
    state(State::Initialize)
{
    SetTimeout(200);
}

DiscoveryModule::~DiscoveryModule()
{

}

void DiscoveryModule::CreateNodes(BaseNode& rootNode)
{
    BaseNode* pNode = rootNode.FindOrCreateChild<BaseNode>("Network");
    pDiscoveryNode = pNode->FindOrCreateChild<BaseNode>("DiscoverySocket");
    pPortNode = pDiscoveryNode->FindOrCreateChild<UInt16Node>("Port", 1974);
    pBoundNode = pDiscoveryNode->FindOrCreateChild<BoolNode>("Bound", false);
    pBoundNode->SetFlag(FlagType::readonly, true);

    pPortNode->Subscribe(this);
}

void DiscoveryModule::Notify()
{
    if (pPortNode->IsValueChanged())
    {
        sockDiscover.Close();
        state = State::Initialize;
    }
}

void DiscoveryModule::Publish()
{
}

void DiscoveryModule::OnTimer() 
{
    switch (state)
    {
    case State::Initialize:
        ;
        if (sockDiscover.Bind(pPortNode->Get(), SocketType::UDP) == 0)
        {
            state = State::Bound;
            pBoundNode->Set(true);
            if (pBoundNode->IsAttributeUsed(infoAttrib.GetID()))
            {
                pBoundNode->SetAttribute(infoAttrib.GetID(), "");
            }
        }
        else
        {
            pBoundNode->SetAttribute(infoAttrib.GetID(), "Failed to bind port.");
        }
        return;
    case State::Bound:

        if (sockDiscover.IsReadPending(0))
        {
            size_t dataLen = buffer.size() - 1;
            SOCKADDR_IN srcAddr;
            int retVal = sockDiscover.RecieveFrom(buffer.data(), &dataLen, srcAddr);
            if (dataLen == 0 || retVal != 0)
            {
                // Error 
                return;
            }

            // Send reply to the broadcast.
            retVal = sockDiscover.SendTo(buffer.data(), &dataLen, srcAddr);
        }
        break;
    }
}