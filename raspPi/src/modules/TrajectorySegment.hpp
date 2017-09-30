#pragma once
#include <Eigen/Dense>

// Abstract base class for trajectory segments.
class TrajectorySegment
{
public:
    TrajectorySegment() {};
    virtual ~TrajectorySegment() {};    
    void Initialize(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float delta_time)
    {
        startPos = start;
        targetPos = target;
        deltaTime = delta_time;
    }
    float GetDeltaTime() const { return deltaTime; }
    virtual void GetPosition(float time, Eigen::Vector3f& position) const = 0;
protected:
    Eigen::Vector3f startPos;
    Eigen::Vector3f targetPos;
    float deltaTime;
};

class LinearTrajectorySegment :public TrajectorySegment
{
public:
    LinearTrajectorySegment() {};
    virtual ~LinearTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        position = startPos + (targetPos - startPos) * time / deltaTime;
    }
private:
};
