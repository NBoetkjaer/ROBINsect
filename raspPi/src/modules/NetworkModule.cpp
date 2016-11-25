#include "NetworkModule.hpp"
#include <iostream>

using namespace std;

NetworkModule::NetworkModule():
    state(State::Initialize)
{

}

NetworkModule::~NetworkModule()
{

}

void NetworkModule::Execute()
{
    switch (state)
    {
    case State::Initialize:
        std::cout << "Discover Bind " << sockDiscover.Bind(1974, SocketType::UDP) << endl;
        std::cout << "Listen Bind " << sockListen.Bind(1973, SocketType::TCP) << endl;
        std::cout << "Listen " << sockListen.Listen() << endl;
        state = State::Listning;
        return;
    case State::Listning:
        if(sockListen.IsReadPending(0))
        {
            std::cout << "Accept " << sockAccept.Accept(sockListen) << endl;
            std::cout << "Shutdown (listen) " << sockListen.Shutdown() << endl;
            std::cout << "Close (listen)" << sockListen.Close() << endl;
            state = State::Connected;
        }
        break;
    case State::Connected:
        if(sockAccept.IsReadPending(0))
        {
            size_t dataLen = buffer.size(); 
            int retVal = sockAccept.Recieve(buffer.data(), &dataLen);
            std::cout << "Recieve " << retVal << ": ";
            if(dataLen == 0)
            {
                // Remote end is shutdown
                std::cout << "Remote end disconnected - Shutdown (sockAccept) " << sockAccept.Shutdown() << endl;
                std::cout << "Close socket" << sockAccept.Close() << endl;
                sockDiscover.Close();
                state = State::Initialize;
                return;
            }
            buffer[dataLen] = 0; // Terminate the string.
            std::cout << buffer.data() << endl;
        }
        break;
    }

    if(sockDiscover.IsReadPending(0))
    {   // Send reply to the broadcast.
    
    }
}