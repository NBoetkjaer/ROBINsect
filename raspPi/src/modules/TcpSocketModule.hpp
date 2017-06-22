#pragma once

#include "Module.hpp"
#include "../communication/Socket.hpp"
#include <array>
#include <string>

class TcpSocketModule : public Module
{
private:
    Int64Node* pRcvNode;
    Int64Node* pSentNode;
    BoolNode* pConnectedNode;
    UInt16Node* pPortNode;
public:
    TcpSocketModule();
    virtual ~TcpSocketModule();
    virtual void Execute() override;
    
protected:
    void CreateNodes(BaseNode& rootNode, uint16_t portNo = 0);
    virtual void DataReceived(const char* pData, size_t dataLen) = 0;

    enum class  State {Initialize, Listning, Connected};
    State state;
    Socket sockListen;
    Socket sockAccept;

    std::array<char, 1024> buffer;
    size_t bufferWritePos;
    size_t bufferEndPos;
};