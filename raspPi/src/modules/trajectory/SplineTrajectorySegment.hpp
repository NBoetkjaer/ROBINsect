#pragma once
#include "TrajectorySegment.hpp"
#include <array>

class SplineTrajectorySegment :public TrajectorySegment
{
public:
    SplineTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, const Eigen::Vector3f& startVel, const Eigen::Vector3f& targetVel, float duration);

    ~SplineTrajectorySegment() override {};

    void GetPosition(float time, Eigen::Vector3f& position) const override;
protected:    
    std::array< Eigen::Vector3f, 4> m_polyCoeff;
};