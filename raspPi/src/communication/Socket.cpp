#include "Socket.hpp"
#include <fcntl.h>

#ifdef _WIN32
#define VALID_SOCKET(socket) ((socket) != INVALID_SOCKET)
#else
#define VALID_SOCKET(socket) ((socket) >= 0)
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> // memset declared here.
#endif

Socket::Socket()
    :socketID(0),
    bytesSent(0),
    bytesRecieved(0),
    isBlocking(true), bound(false), connected(false)
{
}

Socket::~Socket()
{
    if (socketID >= 0)
        Close();
}

int Socket::SetBlocking(bool blocking)
{
    int retVal;
#ifdef WIN32
    u_long nonBlock = blocking ? 0 : 1;
    retVal = ioctlsocket(socketID, FIONBIO, &nonBlock);
#else
    if(blocking){
        retVal = fcntl(socketID, F_SETFL, 0);
    }
    else
    {
        retVal = fcntl(socketID, F_SETFL, O_NONBLOCK);
    }
#endif
    if (retVal < 0)
    {
        isBlocking = blocking;
    }
    return retVal;

}

int Socket::Listen()
{
    if (listen(socketID, SOMAXCONN) < 0)
        return SocketError();
    return 0;
}

int Socket::Bind(uint16_t portNo, SocketType type)
{
    socketType = type;
    SOCKADDR_IN socketAddr;
#ifdef WIN32
    char optval = 1;
#else
    int optval = 1;
#endif

    if (bound)
    {
        Close();
    }

    switch (socketType)
    {
    case SocketType::TCP: //TCP Socket        
        socketID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;
    case SocketType::UDP: //UDP Socket
        socketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;
    }

    if (!VALID_SOCKET(socketID)) // Error socket not created
        return SocketError();

    // SO_REUSEADDR allows app to connect to server
    // port quickly after last shutdown:
    setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = INADDR_ANY;
    socketAddr.sin_port = htons(portNo);

    //Bind socket
    if (bind(socketID, (SOCKADDR *)&socketAddr, sizeof(SOCKADDR_IN)) < 0) // There was an error binding this socket
        return SocketError();
    bound = true;
    if (socketType == SocketType::UDP)
    {
        // Allow broadcast
        if (setsockopt(socketID, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0)
        {
            return SocketError();
        }
    }
    return 0;
}

int Socket::Connect(const SOCKADDR_IN& sockAddr)
{
    if (connect(socketID, (const SOCKADDR *)&sockAddr, sizeof(SOCKADDR_IN)) < 0)
        return SocketError();
    connected = true;
    return 0;
}

int Socket::Accept(const Socket &listningSocket)
{
    SOCKADDR addr;
    SOCKET newSockID;

    socklen_t addrlen = sizeof(SOCKADDR);
    newSockID = accept(listningSocket.socketID, &addr, &addrlen);
    if (!VALID_SOCKET(newSockID))
    {
        return SocketError();
    }
    socketID = newSockID;
    connected = true;
    socketType = SocketType::TCP;
    return 0;
}

int Socket::GetPort(uint16_t *portNo)
{
    SOCKADDR_IN socketAddr;
    socklen_t addrlen = sizeof(SOCKADDR_IN);
    if (getsockname(socketID, (SOCKADDR*) &socketAddr, &addrlen) < 0)
    {
        return SocketError();
    }
    *portNo = ntohs(socketAddr.sin_port);
    return 0;
}

int Socket::Recieve(char *pData, size_t *pDataLen)
{
    int iBytesRecieved;
    iBytesRecieved = recv(socketID, pData, *pDataLen, 0);
    if (iBytesRecieved < 0)
    {
        return SocketError();
    }
    if (iBytesRecieved == 0)
    {
        //if(socketType == SocketType::TCP) shutdown();
        // Remote end is shutdown - only TCP.
        // Return ???
    }
    *pDataLen = iBytesRecieved;
    bytesRecieved += iBytesRecieved;
    return 0;
}

int Socket::Send(const char *pData, size_t *pDataLen)
{
    int iBytesSent;
    iBytesSent = send(socketID, pData, *pDataLen, 0);
    if (iBytesSent < 0)
    {
        return SocketError();
    }
    *pDataLen = iBytesSent;
    bytesSent += iBytesSent;
    return 0;
}

int Socket::RecieveFrom(char *pData, size_t *pDataLen, SOCKADDR_IN& src_addr)
{
    int iBytesRecieved;
    int iAddrLen = sizeof(SOCKADDR_IN);
    iBytesRecieved = recvfrom(socketID, pData, *pDataLen, 0, (SOCKADDR *) &src_addr, &iAddrLen);
    // assert(iAddrLen <= sizeof(SOCKADDR_IN));
    if (iBytesRecieved < 0)
    {
        return SocketError();
    }
    *pDataLen = iBytesRecieved;
    bytesRecieved += iBytesRecieved;

    return 0;
}

int Socket::SendTo(const char *pData, size_t *pDataLen, const SOCKADDR_IN& src_addr)
{
    int iBytesSent;
    iBytesSent = sendto(socketID, pData, *pDataLen, 0,(SOCKADDR *) &src_addr, sizeof(SOCKADDR_IN));
    if (iBytesSent < 0)
    {
        return SocketError();
    }
    *pDataLen = iBytesSent;
    bytesSent += iBytesSent;
    return 0;
}

bool Socket::IsReadPending(uint32_t timeout_us) const
{
    fd_set rRead;
    struct timeval to;
    int iSocketsReady;

    FD_ZERO(&rRead);
    FD_SET(socketID, &rRead);
    to.tv_sec = 0;
    to.tv_usec = timeout_us;

    iSocketsReady = select(socketID + 1, &rRead, NULL, NULL, &to);
    if (iSocketsReady == 1) // &&  FD_ISSET (socketID, &rRead)   
    {
        return true;
    }
    return false;
    //if(iSocketsReady<0)
    //  return SocketError();
}

bool Socket::IsWritePending(uint32_t timeout_us) const
{
    fd_set rWrite;
    struct timeval to;
    int iSocketsReady;

    FD_ZERO(&rWrite);
    FD_SET(socketID, &rWrite);
    to.tv_sec = 0;
    to.tv_usec = timeout_us;

    iSocketsReady = select(socketID + 1, NULL, &rWrite, NULL, &to);
    if (iSocketsReady == 1) // &&  FD_ISSET (socketID, &rWrite)   
    {
        return false; // No write is pending.
    }
    return true;
    //if(iSocketsReady<0)
    //  return SocketError();
}

int Socket::Shutdown()
{
    bound = false;
    connected = false;
#ifdef WIN32
#define SHUT_RDWR SD_BOTH
#endif
    if (shutdown(socketID, SHUT_RDWR) < 0)
    {
        return SocketError();
    }
    return 0;
}

int Socket::Close()
{
    bound = false;
    connected = false;
#ifdef WIN32
    if(closesocket(socketID) < 0)
        return SocketError();
#else
    if (close(socketID) < 0)
        return SocketError();
#endif
    socketID = 0;
    return 0;
}


int Socket::SocketError() const
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
