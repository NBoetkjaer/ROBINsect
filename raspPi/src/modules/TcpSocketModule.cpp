#include "TcpSocketModule.hpp"
#include <ctype.h>
#include <iostream> // ToDo: Remove all debug print ...
#include <cassert>
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
    pPortNode = rootNode.FindOrCreateChild<UInt16Node>("ListenPort", portNo);
    pRcvNode = rootNode.FindOrCreateChild<Int64Node>("BytesRecived");
    pSentNode = rootNode.FindOrCreateChild<Int64Node>("BytesSent");
    pConnectedNode = rootNode.FindOrCreateChild<BoolNode>("Connected");
}

void TcpSocketModule::Execute()
{
    switch (state)
    {
    case State::Initialize:
        std::cout << "Listen Bind " << sockListen.Bind(pPortNode->Get(), SocketType::TCP) << endl;
        std::cout << "Listen " << sockListen.Listen() << endl;
        uint16_t port;
        if (sockListen.GetPort(&port) == 0)
        {
            pPortNode->Set(port);
        }        
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
            Connected();
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

void TcpSocketModule::SendData(const char* pData, size_t dataLen)
{
    if (!sockAccept.IsConnected())
    {
        return;
    }
    const char *pPtr = pData;
    size_t dataSent;
    while (true)
    {
        if (!sockAccept.IsWritePending(10))
        {
            dataSent = dataLen;
            int retVal = sockAccept.Send(pPtr, &dataSent);
            if(retVal != 0)
            {
                return;  // ToDo handle error 
            }
            assert(dataLen >= dataSent); // Should never happen.
            dataLen = dataLen - dataSent;
            pPtr += dataSent;
            if (dataLen == 0) return;
        }
    }

}
