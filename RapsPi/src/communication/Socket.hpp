//http://stackoverflow.com/questions/28027937/cross-platform-sockets
#include <cstdint>
#include <sys/types.h> 
#include <sys/socket.h>
#include <errno.h>

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

enum ESocketType {UDP, TCP};
typedef enum ESocketType SocketType;

class Socket
{
public:
    Socket(SocketType type);
    virtual ~Socket();

    int Listen();
    int Bind(uint16_t portNo);
    int Connect(const SOCKADDR_IN& sockAddr);
    int Accept(const Socket &listningSocket);
    int Shutdown();
    int Close();
    
    int Recieve(char *pData, size_t *pDataLen);
    int Send(const char *pData, size_t *pDataLen);

    bool IsReadPending(uint32_t timeout_us) const;
    bool IsWritePending(uint32_t timeout_us) const;

    int SetBlocking(bool blocking);
    int64_t GetBytesSent() const {return bytesSent;};
	int64_t GetBytesRecieved() const {return bytesRecieved;};
    bool IsBound(){return bound;};
    bool IsConnected(){return connected;};
private:
    
    int SocketError(){return errno;};
    SOCKET  socketID;
    int64_t bytesSent, bytesRecieved;    
    SocketType socketType;
    bool isBlocking;
    bool bound;
    bool connected;
};