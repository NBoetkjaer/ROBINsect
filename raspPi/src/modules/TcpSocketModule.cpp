#include "TcpSocketModule.hpp"
#include <ctype.h>
#include <iostream> // ToDo: Remove all debug print ...

using namespace std;

TcpSocketModule::TcpSocketModule():
    state(State::Initialize)
{
}

TcpSocketModule::~TcpSocketModule()
{
}

void TcpSocketModule::CreateNodes(BaseNode& rootNode, uint16_t portNo)
{
    pPortNode = rootNode.FindOrCreateChild<UInt16Node>("Port", portNo);
    pRcvNode = rootNode.FindOrCreateChild<Int64Node>("BytesRecived");
    pSentNode = rootNode.FindOrCreateChild<Int64Node>("BytesSent");
    pConnectedNode = rootNode.FindOrCreateChild<BoolNode>("Connected");
}

void TcpSocketModule::Execute()
{
    switch (state)
    {
    case State::Initialize:
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
            std::cout << "Recieve " << retVal << ": " << "Total bytes : " << sockAccept.GetBytesRecieved() << std::endl;
            if(dataLen == 0 || retVal != 0)
            {
                // Remote end is shutdown
                std::cout << "Remote end disconnected - Shutdown (sockAccept) " << sockAccept.Shutdown() << endl;
                std::cout << "Close socket" << sockAccept.Close() << endl;
                state = State::Initialize;
                return;
            }
            DataReceived(buffer.data(), dataLen);
            pRcvNode->Set(sockAccept.GetBytesRecieved());
        }
        pSentNode->Set(sockAccept.GetBytesSent());
        break;
    }
}
