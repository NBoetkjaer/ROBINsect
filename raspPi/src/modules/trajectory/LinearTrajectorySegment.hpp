#pragma once
#include "TrajectorySegment.hpp"

class LinearTrajectorySegment :public SimpleTrajectorySegment
{
public:
    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration):
        SimpleTrajectorySegment(duration), m_startPos(start)
    {
        m_slope = (target - m_startPos) / m_duration_s;
    };

    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration, float startVelocity, float endVelocity) :
        SimpleTrajectorySegment(duration), m_startPos(start)
    {
        m_slope = (target - m_startPos) / m_duration_s;
        UseSpline(startVelocity, endVelocity);
    };

    virtual ~LinearTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        if (m_bSpline)
        {
            time = GetSplineTime(time);
        }
        position = m_startPos + m_slope * time;
    };
protected:
    virtual float GetCurveLength() const override
    {
        return (m_slope*m_duration_s).norm();
    };
    Eigen::Vector3f m_startPos;
    Eigen::Vector3f m_slope;
};