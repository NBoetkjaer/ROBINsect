#pragma once

class Gait
{
public:
    Gait() = delete; // No default constructor.
    virtual ~Gait() {};
};


class TripodGait : public Gait
{

};

class RippleGait : public Gait
{

};