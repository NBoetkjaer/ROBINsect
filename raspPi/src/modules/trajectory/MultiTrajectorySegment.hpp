#pragma once
#include <memory>
#include <vector>
#include <variant>

#include "TrajectorySegment.hpp"

class MultiTrajectorySegment :public TrajectorySegment
{
public:
    MultiTrajectorySegment():
        TrajectorySegment(0.0f)
    {
    };
    virtual ~MultiTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override;

    void PushSegment(std::unique_ptr<TrajectorySegment> segment);
    //std::unique_ptr<TrajectorySegment>  PopSegement();
protected:
private:
    std::vector<std::unique_ptr<TrajectorySegment>> m_trajectories;
    
};

