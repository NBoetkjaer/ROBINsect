#include "gait.hpp"

#include "../trajectory/MultiTrajectorySegment.hpp"
#include "../trajectory/LinearTrajectorySegment.hpp"
#include "../trajectory/CircularTrajectorySegment.hpp"
#include "../trajectory/SplineTrajectorySegment.hpp"


using namespace Geometry3D;

bool Gait::SetState(GaitStates newState)
{
    m_state = newState;
    //switch (newState)
    //{
    //case GaitStates::Stopped:
    //    if (m_state != GaitStates::Stopping) return false;
    //    // Check that all legs have an initial position.
    //    m_state = newState;
    //    break;
    //case GaitStates::Initializing:
    //    if (m_state != GaitStates::Stopped) return false;
    //    // Check that all legs have an initial position.
    //    m_state = newState;
    //    break;
    //case GaitStates::Initialized:
    //    if (m_state != GaitStates::Initializing) return false;
    //    // Check that all legs have an initial position.
    //    m_state = newState;
    //    break;

    //case GaitStates::Active:
    //    if (m_state != GaitStates::Initializing) return false;
    //    break;
    //case GaitStates::Stopping:
    //    // Check that all legs have an initial position.
    //    break;
    //default:
    //    return false;
    //}
    return true;
}

void Gait::GetLegRange(int legId, Eigen::Vector3f& cntrPos, float& range) const
{
    if (legId < 0 || legId >= m_legCentralPos.size())
        return;
    cntrPos = m_legCentralPos[legId];
    range = m_legRange[legId];

}

void Gait::SetLegRange(int legId, const Eigen::Vector3f& cntrPos, float range)
{
    if (legId < 0 || legId >= m_legCentralPos.size())
        return;
    m_legCentralPos[legId] = cntrPos;
    m_legRange[legId] = range;
}

void Gait::SetLegPosition(int legId, const Eigen::Vector3f& pos)
{
    if (legId < 0 || legId >= m_legPos.size())
        return;
    m_legPos[legId] = pos;
}

void Gait::GetLegPosition(int legId, Eigen::Vector3f& pos) const
{
    if (legId < 0 || legId >= m_legPos.size())
        return;
    pos = m_legPos[legId];
}


std::unique_ptr<TrajectorySegment> Gait::CreateCircularStepTrajectory(const Eigen::Vector3f& circleCntr, const Eigen::Vector3f& pos, const Eigen::Vector3f& upVec, float arcAngle_rad, float lift, float fullCycleTime, float stepCycleRatio)
{
    // circleCntr is center of the circle path, and is located on the ground.
    // pos is center position on the "ground"
    // upVec is a vector pointing up from the ground/circle plane (- on same side as the circle normal)
    // The step is composed of two circular segments: a step segment and a retraction segment.
    // The two segments are connected by two other circular segments.
    Eigen::Vector3f radiusUnitVec = (pos - circleCntr);
    float radius = radiusUnitVec.norm();
    radiusUnitVec.normalize();

    float retractFrac = 1.0f - stepCycleRatio;
    float stepFrac = stepCycleRatio;
    float stepLen = abs(arcAngle_rad * radius);
    float arcLen = (float)M_PI * abs(lift); // Length of both connecting arc segments.
    float velStep = stepLen / (stepFrac * fullCycleTime); // Constant velocity of step.
    float velRetract = (stepLen + arcLen) / (retractFrac * fullCycleTime); // Avarage velocity of retraction.
    float arcTime = retractFrac * fullCycleTime * 0.5f * arcLen / (stepLen + arcLen);
    float retractTime = retractFrac * fullCycleTime - 2.0f * arcTime;


    // Circle chord formula: http://mathworld.wolfram.com/CircularSegment.html
    float chordLen = 2.0f * radius * sin(0.5f * arcAngle_rad);
    // distance from center to chord line
    float r = 0.5f * sqrt(4 * radius * radius - chordLen * chordLen);
    // Chord unit vector
    Eigen::Vector3f chordUnitVec = radiusUnitVec.cross(upVec);
    chordUnitVec.normalize(); // In case the supplied normal is not normalized.
    // Intersection of line cntr<->pos with chord. 
    Eigen::Vector3f chordIntersec = circleCntr + r * radiusUnitVec;
    Eigen::Vector3f circleNormal = radiusUnitVec.cross(chordUnitVec);

    std::unique_ptr<MultiTrajectorySegment> multiTrajectory = std::make_unique<MultiTrajectorySegment>();

    Eigen::Vector3f start = chordIntersec - 0.5f * chordLen * chordUnitVec;
    Eigen::Vector3f end = chordIntersec + 0.5f * chordLen * chordUnitVec;
    std::unique_ptr<TrajectorySegment> cirlceTrajectory1 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, fullCycleTime * stepFrac, velStep, velStep);
    multiTrajectory->PushSegment(std::move(cirlceTrajectory1));

    start = end;
    end -= circleNormal * lift;
    Eigen::Vector3f normal = circleCntr - start;
    normal.normalize();
    std::unique_ptr<TrajectorySegment> circleTrajectory2 = std::make_unique<CircularTrajectorySegment>(start, end, normal, 0.5f * lift, arcTime, velStep, velStep);
    multiTrajectory->PushSegment(std::move(circleTrajectory2));

    start = end;
    end -= chordLen * chordUnitVec;
    auto startVel = velStep * upVec.cross(start - circleCntr).normalized();
    auto endVel = velStep * upVec.cross(end - circleCntr).normalized();
    std::unique_ptr<TrajectorySegment> splineTrajectory3 = std::make_unique<SplineTrajectorySegment>(start, end, startVel, endVel, retractTime);
    multiTrajectory->PushSegment(std::move(splineTrajectory3));

    //std::unique_ptr<TrajectorySegment> circleTrajectory3 = std::make_unique<CircularTrajectorySegment>(start, end, -circleNormal, radius, retractTime, velRetract, velRetract);
    //multiTrajectory->AddSegment(std::move(circleTrajectory3));

    start = end;
    end += circleNormal * lift;
    normal = circleCntr - end;
    normal.normalize();
    std::unique_ptr<TrajectorySegment> circleTrajectory4 = std::make_unique<CircularTrajectorySegment>(start, end, normal, 0.5f * lift, arcTime, velStep, velStep);
    multiTrajectory->PushSegment(std::move(circleTrajectory4));

    return std::move(multiTrajectory);
}

std::unique_ptr<TrajectorySegment> Gait::CreateLinearStepTrajectory(const Eigen::Vector3f& step, const Eigen::Vector3f& pos, float lift, float fullCycleTime, float stepCycleRatio)
{
    // Assumes that the z-axis i up.
    // pos is center position on the "ground"
    // The step is composed of two linear segments connected with circular paths at the end.
    //   ______________________     -
    //  /                      \    |
    // (          pos           )   | lift
    //  \__________*___________/    _
    //   <......step..........>
    //
    // The fullCycleTime is divided in two: The step time, (fullCycleTime * stepCycleRatio) and the remaing part, the retraction  (fullCycleTime * (1.0f - stepCycleRatio)).
    // The velocity is stepLen/(fullCycleTime * 3/4 ) 
    //

    Eigen::Vector3f circleNormal = step.cross(Eigen::Vector3f::UnitZ());
    circleNormal.normalize();

    float radius = 0.5f * lift;
    float arcLen = (float)M_PI * lift; // Length of both arc segments.
    float stepLen = step.norm();
    float retractFrac = 1.0f - stepCycleRatio;
    float stepFrac = stepCycleRatio;
    float velStep = stepLen / (stepFrac * fullCycleTime); // Constant velocity of step.
    float velRetract = (stepLen + arcLen) / (retractFrac * fullCycleTime); // Avarage velocity of retraction.
    float arcTime = retractFrac * fullCycleTime * 0.5f * arcLen / (stepLen + arcLen);
    float retractTime = retractFrac * fullCycleTime - 2.0f * arcTime;
    std::unique_ptr<MultiTrajectorySegment> multiTrajectory = std::make_unique<MultiTrajectorySegment>();

    Eigen::Vector3f start = pos - step * 0.5f;
    Eigen::Vector3f end = pos + step * 0.5f;
    std::unique_ptr<TrajectorySegment> lineTrajectory1 = std::make_unique<LinearTrajectorySegment>(start, end, fullCycleTime * stepFrac, velStep, velStep);
    multiTrajectory->PushSegment(std::move(lineTrajectory1));

    start = end;
    end = end + Eigen::Vector3f::UnitZ() * lift;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory2 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, arcTime, velStep, velRetract);
    multiTrajectory->PushSegment(std::move(cirldeTrajectory2));

    start = end;
    end = end - step;
    Eigen::Vector3f stepUnit = step / stepLen;
    std::unique_ptr<TrajectorySegment> splineTrajectory3 = std::make_unique<SplineTrajectorySegment>(start, end, -velRetract * stepUnit, -velRetract * stepUnit, retractTime);
    multiTrajectory->PushSegment(std::move(splineTrajectory3));

    start = end;
    end = end - Eigen::Vector3f::UnitZ() * lift;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory4 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, arcTime, velRetract, velStep);
    multiTrajectory->PushSegment(std::move(cirldeTrajectory4));

    return std::move(multiTrajectory);
}