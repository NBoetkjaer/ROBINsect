#pragma once

#include "Module.hpp"
#include "../communication/Socket.hpp"
#include <array>

class NetworkModule : public Module
{
public:
    NetworkModule();
    virtual ~NetworkModule();
    virtual void Execute();
private:
    enum class  State {Initialize, Listning, Connected};
    State state;
    Socket sockDiscover;
    Socket sockListen;
    Socket sockAccept;
    std::array<char, 1024> buffer;
};