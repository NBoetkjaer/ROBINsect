//http://stackoverflow.com/questions/28027937/cross-platform-sockets
#pragma once

#include <cstdint>
#ifndef _WIN32
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <errno.h>

    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
#else
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif

enum class SocketType {UDP, TCP};

class Socket
{
public:
#ifdef _WIN32
    // Utillity functions used to initialize WinSock.
    static int InitLib(byte majorVer, byte minorVer){ WSADATA wsaData; return WSAStartup(MAKEWORD(majorVer, minorVer),& wsaData); };
    static int ExitLib(){ return WSACleanup(); };
#endif
    Socket();
    virtual ~Socket();

    int Listen();
    int Bind(uint16_t portNo, SocketType type);
    int Connect(const SOCKADDR_IN& sockAddr);
    int Accept(const Socket &listningSocket);
    int GetPort(uint16_t *portNo);
    int Shutdown();
    int Close();
    
    int Recieve(char *pData, size_t *pDataLen);
    int Send(const char *pData, size_t *pDataLen);

    int RecieveFrom(char *pData, size_t *pDataLen, SOCKADDR_IN& src_addr);
    int SendTo(const char *pData, size_t *pDataLen, const SOCKADDR_IN& src_addr);

    bool IsReadPending(uint32_t timeout_us) const;
    bool IsWritePending(uint32_t timeout_us) const;

    int SetBlocking(bool blocking);
    int64_t GetBytesSent() const {return bytesSent;};
	int64_t GetBytesRecieved() const {return bytesRecieved;};
    bool IsBound() const {return bound;};
    bool IsConnected() const {return connected;};
private:
    
    int SocketError() const;

    SOCKET  socketID;
    int64_t bytesSent;
    int64_t bytesRecieved;
    SocketType socketType;
    bool isBlocking;
    bool bound;
    bool connected;
};