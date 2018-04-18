#pragma once
#include <memory>
#include <vector>
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
    void AddSegment(std::unique_ptr<TrajectorySegment> segment);
protected:
private:
    std::vector<std::unique_ptr<TrajectorySegment>> trajectories;
    
};