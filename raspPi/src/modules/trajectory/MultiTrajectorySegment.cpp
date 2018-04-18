#include "MultiTrajectorySegment.hpp"

void MultiTrajectorySegment::AddSegment(std::unique_ptr<TrajectorySegment> segment)
{
    durationTime += segment->GetDuration();
    trajectories.push_back(std::move(segment));
}

void MultiTrajectorySegment::GetPosition(float time, Eigen::Vector3f& position) const
{
    // Locate active trajectory segment index. 
    if(trajectories.empty()) return;
    const int numSegments = trajectories.size();
    int segIdx = 0;
    float accumulatedTime = trajectories[segIdx]->GetDuration();
    while (accumulatedTime < time && segIdx < numSegments)
    {
        segIdx++;
        accumulatedTime += trajectories[segIdx]->GetDuration();
    }
    TrajectorySegment* pSegment = (segIdx >= numSegments) ? trajectories.back().get(): trajectories[segIdx].get();
    // Convert global trajectory time to active segment time.
    return pSegment->GetPosition(time - (accumulatedTime - pSegment->GetDuration()), position);

}
