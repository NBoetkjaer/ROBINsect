#include "RippleGait.hpp" 

#include "../trajectory/MultiTrajectorySegment.hpp"
#include "../trajectory/LinearTrajectorySegment.hpp"
#include "../trajectory/CircularTrajectorySegment.hpp"
#include "../trajectory/SplineTrajectorySegment.hpp"

#include <algorithm>
#include <numeric>

using namespace Geometry3D;

//  _________/¨\  * Full cycle is 12 ticks
//  ¨\_________/  * Example pattern: 1/6 delay between legs, step/retract ratio 3 (= 9/3)
//  _/¨\________  * Two legs will be overlapping and lifted simultanously.
//  ___/¨\______  * One leg about to touch the ground the other lifting from ground
//  _____/¨\____  * The two legs are placed on each side of the robot (left/right), and
//  _______/¨\__  * in a pattern that assures that neither the two front legs nor the two back legs 
//                * are lifted simultaneously.

void RippleGait::UpdateLegPositions() 
{
    int seqId = 0;
    for (auto legId : m_legSequence)
    {
        if (m_curTrajectory[legId])
        {
            float time = m_deltaTime_s + PhaseOffset(seqId);
            time = time >= m_stepDuration_s ? time - m_stepDuration_s : time;
            m_curTrajectory[legId]->GetPosition(time, m_legPos[legId]);
        }
        seqId++;
    }
}

float RippleGait::PhaseOffset(int seqId)
{
    return seqId * m_stepDuration_s / NUM_LEGS;
}

void RippleGait::PrepareLegPositions()
{
    for (int idx = 0; idx < m_curTrajectory.size(); idx++)
    {
        if (m_curTrajectory[idx])
        {
            float time = std::min(m_curTrajectory[idx]->GetDuration(), m_deltaTime_s);
            m_curTrajectory[idx]->GetPosition(time, m_legPos[idx]);
        }
    }    
}

void RippleGait::InitLegSeqence()
{
    m_legSequence =
    {
        LEG_FRONT_LEFT,
        LEG_MID_RIGHT,
        LEG_MID_LEFT,
        LEG_BACK_RIGHT,
        LEG_FRONT_RIGHT,
        LEG_BACK_LEFT,
    };
}

void RippleGait::CreateWalkingTrajectory(LegTrajectories& trajectories)
{
    for(auto &traj :trajectories)
    {
        traj = nullptr;
    }

    const float stepCycleRatio = 3.0f / 4.0f;
    float dir = m_velocity >= 0 ? 1.0f : -1.0f;
    if (m_angularVelocity_rad == 0.0f)
    {
        if (m_velocity == 0.0f)
            return;
        m_stepDuration_s = abs(m_stepVector.norm() / m_velocity / stepCycleRatio);
        
        for (auto legId : m_legSequence)
        {
            trajectories[legId] = CreateLinearStepTrajectory(dir * m_stepVector, m_stepCntr[legId], m_stepHeight, m_stepDuration_s, stepCycleRatio);
        }
    }
    else
    {
        Eigen::Vector3f groundCntr;
        m_groundPlane.DistanceToPoint(Eigen::Vector3f::Zero(), groundCntr);

        // arcLen = radius * arcAng.
        float bodyRadius = m_velocity / m_angularVelocity_rad;
        Eigen::Vector3f pivot = m_stepVector.cross(m_groundPlane.Normal());
        pivot *= bodyRadius / pivot.norm();
        pivot += groundCntr;
        auto maxRadius = std::reduce(m_stepCntr.begin(), m_stepCntr.end(), 0.0f, [&](float dist, const auto& pt) {return std::max(dist, (pt - pivot).norm()); });
        // Formula using the chord length 'm_stepVector.norm()' of the leg with the largest radius.
        float arcAngle = 2.0f * asin(m_stepVector.norm() / (2 * maxRadius));
        dir = m_angularVelocity_rad >= 0.0f ? 1.0f : -1.0f;
        m_stepDuration_s = abs(arcAngle / m_angularVelocity_rad / stepCycleRatio);
        // Approximation of minimum duration, when retracting in remaining time using maximum allowed leg tip speed.
        float minDuration = abs(maxRadius * arcAngle + M_PI * m_stepHeight) / (m_maxLegVelocity * (1 - stepCycleRatio));
        if (m_stepDuration_s < minDuration)
        {
            m_stepDuration_s = minDuration;
        }


        for (auto legId : m_legSequence)
        {
            trajectories[legId] = CreateCircularStepTrajectory(pivot, m_stepCntr[legId], dir * m_groundPlane.Normal(), arcAngle, dir * m_stepHeight, m_stepDuration_s, stepCycleRatio);
        }
    }
}

void RippleGait::CreateInitialTrajectory(LegTrajectories& initTrajectories, LegTrajectories& gaitTrajectories)
{
    // The initialization sequence will place the legs on their initial step position. 
    // The legs are moved three at at time. First the legs in the air and thereafter the rest.
    int onGroundCounter = 0;
    float duration = 4.0f;
    int seqId = 0;
    for (auto legId : m_legSequence)
    {
        Eigen::Vector3f dest = m_stepCntr[legId];
        if (gaitTrajectories[legId])
        {
            gaitTrajectories[legId]->GetPosition(PhaseOffset(seqId), dest);
        }
        // Check if leg is on the ground 
        if (abs(m_groundPlane.DistanceToPoint(m_legPos[legId])) < 0.001f && onGroundCounter < 3)
        {
            onGroundCounter++;
            // Create a three segment trajectory. (first segment is a pause, to wait for the other legs.)
            auto traj = std::make_unique< MultiTrajectorySegment >();
            auto viaPoint = dest + m_groundPlane.Normal() * m_stepHeight;
            auto seg0 = std::make_unique < LinearTrajectorySegment >(m_legPos[legId], m_legPos[legId], duration/ 2.0f);
            auto seg1 = std::make_unique < LinearTrajectorySegment >(m_legPos[legId], viaPoint, duration/4.0f, 0.0f, 0.0f);
            auto seg2 = std::make_unique < LinearTrajectorySegment >(viaPoint, dest, duration/4.0f, 0.0f, 0.0f);
            traj->PushSegment(std::move(seg0));
            traj->PushSegment(std::move(seg1));
            traj->PushSegment(std::move(seg2));
            initTrajectories[legId] = std::move(traj);
        }
        else
        {
            // Create a two segment trajectory.
            auto traj = std::make_unique< MultiTrajectorySegment >();
            auto viaPoint = dest + m_groundPlane.Normal() * m_stepHeight;
            auto seg0 = std::make_unique < LinearTrajectorySegment >(m_legPos[legId], viaPoint, duration/4.0f, 0.0f, 0.0f);
            auto seg1 = std::make_unique < LinearTrajectorySegment >(viaPoint, dest, duration /4.0f, 0.0f, 0.0f);
            traj->PushSegment(std::move(seg0));
            traj->PushSegment(std::move(seg1));
            initTrajectories[legId] = std::move(traj);
        }        
        seqId++;
    }
}

void RippleGait::UpdateGroundPlane()
{
    // Determine the leg distance from plane of the instect (x-y plane)
    Plane3D_32f bodyPlane({ 0,0,1 }, 0);
    std::array<float, NUM_LEGS> dist;
    std::array<int, NUM_LEGS> index;

    std::transform(m_legPos.begin(), m_legPos.end(), dist.begin(), [&bodyPlane](const auto& leg) {return bodyPlane.DistanceToPoint(leg); });

    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [&dist](int i1, int i2) {return dist[i1] < dist[i2]; });

    auto* p0 = &m_legPos[index[0]];
    auto* p1 = &m_legPos[index[1]];
    auto* p2 = &m_legPos[index[2]];
    bool sameSide = !(IS_LEFT_LEG(index[0]) ^ IS_LEFT_LEG(index[1])) && !(IS_LEFT_LEG(index[1]) ^ IS_LEFT_LEG(index[2]));
    if (sameSide)
    {
        p2 = &m_legPos[index[3]];
    }
    // The 3 points farthest away from the body defines the ground plane. (general a bad strategy :-) the points may be on a line)
    // ToDo: Refine the calculation, taking the center of gravity into consideration.
    auto normal = (*p0 - *p1).cross(*p0 - *p2);
    normal *= std::signbit(normal[2]) ? -1.0f : 1.0f; // Make sure the ground plane has a normal pointing upwards (positive Z-value).
    m_groundPlane = Plane3D_32f(normal, m_legPos[index[0]]);

}

void RippleGait::UpdateStep()
{
    // The range of a leg is given as sphere with the center given by m_legCentralPos
    // and the radius given by m_legRange.
    // For all legs find the circle of the intersection between the sphere and the ground plane and return the minimum radius.
    float minRadius = std::numeric_limits<float>::max();
    float radius;
    for (int idx = 0; idx < NUM_LEGS; ++idx)
    {
        auto dist = abs(m_groundPlane.DistanceToPoint(m_legCentralPos[idx], m_stepCntr[idx]));
        if (dist >= m_legRange[idx])
            radius = 0.0f;
        else
            radius = sqrt(m_legRange[idx] * m_legRange[idx] - dist * dist);

        minRadius = std::min(minRadius, radius);
    }
    float stepLength = std::min(m_stepLength, 2.0f * minRadius);
    // Get orientation of the step as a vector on the ground plane.
    m_stepVector = m_groundPlane.ProjectVectorOntoPlane({ sin(m_stepDir_rad), cos(m_stepDir_rad), 0.0f });
    // Change sign since the step i moving oppisite the body.
    m_stepVector *= -stepLength/ m_stepVector.norm();
}

void RippleGait::Reset()
{
    // Run initialization sequence.
    InitLegSeqence();
    UpdateGroundPlane();    // Update the locate of the ground plane.
    UpdateStep();
    // Determine the gait trajectories.
    CreateWalkingTrajectory(m_nextTrajectory);
    // Bring all legs to the ground close to their respective trajectory start.
    CreateInitialTrajectory(m_curTrajectory, m_nextTrajectory); 
    m_deltaTime_s = 0.0f;
}

void RippleGait::UpdateGait(float elapsedTime_s)
{
    m_deltaTime_s += elapsedTime_s;
    
    if (m_state == GaitStates::Stopped)
    {
        //Start();
    }

    if (m_state == GaitStates::Initializing)
    {
        PrepareLegPositions();

        bool done = true;
        for (auto& traj : m_curTrajectory)
        {
            done &= m_deltaTime_s > traj->GetDuration();
        }
        if(done)
        {
            SetState(GaitStates::Active);
            m_deltaTime_s = 0.0f;
            m_curTrajectory.swap(m_nextTrajectory);
        }
    }

    if (m_state == GaitStates::Active)
    {
        // Let gait cycle wrap around to start.
        if (m_deltaTime_s >= m_stepDuration_s)
        {
            m_deltaTime_s -= m_stepDuration_s;
        }
        UpdateLegPositions();
    }

    if (m_state == GaitStates::Stopping)
    {

    }
}
