#pragma once
#include <functional>

class NodeObserver
{
public:
    virtual void Notify() = 0;
};

class Observer :public NodeObserver 
{
public:
    Observer() {}
    virtual void Notify()
    {
        func();
    };

    void SetNotifyFunc(const std::function<void()>  &notify)
    {
        func = notify;
    }

private:
    std::function<void()>  func;
};