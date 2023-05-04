#include "SplineTrajectorySegment.hpp"



SplineTrajectorySegment::SplineTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, const Eigen::Vector3f& startVel, const Eigen::Vector3f& targetVel, float duration):
    TrajectorySegment(duration)
{
    float dur2 = duration * duration;
    float dur3 = duration * duration * duration;

    m_polyCoeff[0] = start;
    m_polyCoeff[1] = startVel;
    m_polyCoeff[2] = 3 * (target - start)/dur2 - 2*startVel/duration - targetVel/duration;
    m_polyCoeff[3] = -2 * (target - start) / dur3 + (targetVel + startVel)/dur2;
};


void SplineTrajectorySegment::GetPosition(float time, Eigen::Vector3f& position) const
{
    const float t2 = time * time;
    const float t3 = time * time * time;
    position = m_polyCoeff[3] * t3 + m_polyCoeff[2] * t2 + m_polyCoeff[1] * time + m_polyCoeff[0];
}