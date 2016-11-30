#pragma once



class Module
{
public:
    virtual void Init() {};
    virtual void Execute() = 0;
};