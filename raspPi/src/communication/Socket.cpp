#include "Socket.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define VALID_SOCKET(socket) socket >= 0
// #define IPPROTO_TCP 0
// #define IPPROTO_UDP 0


Socket::Socket()
    :socketID(0),
     bytesSent(0),
     bytesRecieved(0),
     isBlocking(true), bound(false), connected(false)
{
}

Socket::~Socket()
{
    if(socketID >= 0)
        Close();
}

int Socket::SetBlocking(bool blocking)
{
    int retVal;
    if(blocking){
        retVal = fcntl(socketID, F_SETFL, 0);
    }
    else{
        retVal = fcntl(socketID, F_SETFL, O_NONBLOCK);
    }
    if(retVal <  0)
    { isBlocking = blocking;}
    return retVal;
}

int Socket::Listen()
{    
	if(listen(socketID, SOMAXCONN) < 0)
		return SocketError();
	return 0;    
}

int Socket::Bind(uint16_t portNo, SocketType type)
{
    socketType = type;
	SOCKADDR_IN socketAddr;
	char optval = 1;
	if(bound)
    {
        Close();
    }
    
    switch(socketType)
    {
        case SocketType::TCP: //TCP Socket        
            socketID = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
            break;
        case SocketType::UDP: //UDP Socket
            socketID = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            break;
    }
	
    if (!socketID) // Error socket not created
        return SocketError();

    // SO_REUSEADDR allows app to connect to server
    // port quickly after last shutdown:
    setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = INADDR_ANY; 
    socketAddr.sin_port = htons (portNo);

	//Bind socket
    if (bind (socketID, (SOCKADDR *)&socketAddr, sizeof(SOCKADDR_IN)) < 0 ) // There was an error binding this socket
		return SocketError();
    bound = true;
    if(socketType == SocketType::UDP)
    {
        // Allow broadcast
	    if (setsockopt(socketID, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0 )
	    {
		    return SocketError();
	    }	
    }    
	return 0;
}

int Socket::Connect(const SOCKADDR_IN& sockAddr)
{
	if(connect(socketID, (const SOCKADDR *) &sockAddr, sizeof(SOCKADDR_IN)) < 0)
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
	if(newSockID < 0)
	{
		return SocketError();
	}
	else
	{		
		socketID = newSockID;
        connected = true;
        socketType = SocketType::TCP;
	}
	return 0;
}

int Socket::Recieve(char *pData, size_t *pDataLen)
{
	int iBytesRecieved;
	iBytesRecieved = recv(socketID, pData, *pDataLen, 0);
    if(iBytesRecieved < 0 )
	{
		return SocketError();
	}
    if(iBytesRecieved == 0)
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
{   int iBytesSent;
    iBytesSent = send (socketID, pData, *pDataLen, 0);
    if(iBytesSent < 0 )
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

	FD_ZERO (&rRead);
	FD_SET (socketID, &rRead);	
    to.tv_sec = 0;
    to.tv_usec = timeout_us;

	iSocketsReady = select (socketID + 1, &rRead, NULL, NULL, &to);
	if (iSocketsReady == 1  ) // &&  FD_ISSET (socketID, &rRead)   
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

	FD_ZERO (&rWrite);
	FD_SET (socketID, &rWrite);	
    to.tv_sec = 0;
    to.tv_usec = timeout_us;

	iSocketsReady = select (socketID + 1, NULL, &rWrite, NULL, &to);
	if (iSocketsReady == 1  ) // &&  FD_ISSET (socketID, &rWrite)   
	{		
        return true;
	}
    return false;
    //if(iSocketsReady<0)
    //  return SocketError();
}

int Socket::Shutdown()
{
    bound = false;
    connected = false;
    if(shutdown(socketID, SHUT_RDWR) < 0)
    {
        return SocketError();
    }
    return 0;
}

int Socket::Close() 
{ 
    bound = false;
    connected = false;
    if(close(socketID) < 0)
    	return SocketError();
    socketID = 0;
	return 0;
}
