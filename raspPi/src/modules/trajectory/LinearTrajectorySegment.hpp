#pragma once
#include "TrajectorySegment.hpp"

class LinearTrajectorySegment :public SimpleTrajectorySegment
{
public:
    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration):
        SimpleTrajectorySegment(duration), startPos(start)
    {
        slope = (target - startPos) / durationTime;
    };

    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration, float startVelocity, float endVelocity) :
        SimpleTrajectorySegment(duration), startPos(start)
    {
        slope = (target - startPos) / durationTime;
        UseSpline(startVelocity, endVelocity);
    };

    virtual ~LinearTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        if (bSpline)
        {
            time = GetSplineTime(time);
        }
        position = startPos + slope * time;
    };
protected:
    virtual float GetCurveLength() const override
    {
        return (slope*durationTime).norm();
    };
    Eigen::Vector3f startPos;
    Eigen::Vector3f slope;
};