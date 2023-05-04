#include "MultiTrajectorySegment.hpp"

void MultiTrajectorySegment::PushSegment(std::unique_ptr<TrajectorySegment> segment)
{
    m_duration_s += segment->GetDuration();
    m_trajectories.push_back(std::move(segment));
}

//std::unique_ptr<TrajectorySegment>  MultiTrajectorySegment::PopSegement()
//{
//    std::unique_ptr<TrajectorySegment> ret = std::move(m_trajectories.front());
//    m_trajectories.pop_back();
//    m_duration_s -= ret->GetDuration();
//    return ret;
//}

void MultiTrajectorySegment::GetPosition(float time, Eigen::Vector3f& position) const
{
    // Locate active trajectory segment index. 
    if(m_trajectories.empty()) return;
    const int numSegments = m_trajectories.size();
    int segIdx = 0;
    float accumulatedTime = 0.0f;
    while (accumulatedTime <= time && segIdx < numSegments)
    {        
        accumulatedTime += m_trajectories[segIdx]->GetDuration();
        segIdx++;
    } 
    
    TrajectorySegment* pSegment = (segIdx >= numSegments) ? m_trajectories.back().get(): m_trajectories[segIdx - 1].get();
    // Convert global trajectory time to active segment time.
    return pSegment->GetPosition(time - (accumulatedTime - pSegment->GetDuration()), position);

}
