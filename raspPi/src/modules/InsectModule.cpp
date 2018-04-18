#include "InsectModule.hpp"
#include <cstdlib>
#include "trajectory/MultiTrajectorySegment.hpp"
#include "trajectory/LinearTrajectorySegment.hpp"
#include "trajectory/CircularTrajectorySegment.hpp"
using namespace std;

std::unique_ptr<TrajectorySegment> CreateCircularStepTrajectory(const Eigen::Vector3f& circleCntr, const Eigen::Vector3f& pos, const Eigen::Vector3f& upVec, float arcAngle_rad, float lift, float fullCycleTime)
{
    // circleCntr is center of the circle path, and is located on the ground.
    // pos is center position on the "ground"
    // upVec is a vector pointing up from the ground/circle plane (- on same side as the circle normal)
    // The step is composed of two circular segments: a step segment and a retraction segment.
    // The two segments are connected by two other circular segments.
    Eigen::Vector3f radiusUnitVec = (pos - circleCntr);
    float radius = radiusUnitVec.norm();
    radiusUnitVec.normalize();
    
    float retractFrac = 0.25f;
    float stepFrac = 0.75f;
    float stepLen = abs(arcAngle_rad*radius);
    float arcLen = (float)M_PI * abs(lift); // Length of both connecting arc segments.
    float velStep = stepLen / (stepFrac * fullCycleTime); // Constant velocity of step.
    float velRetract = (stepLen + arcLen) / (retractFrac * fullCycleTime); // Avarage velocity of retraction.
    float arcTime = retractFrac * fullCycleTime * 0.5f * arcLen / (stepLen + arcLen);
    float retractTime = retractFrac * fullCycleTime - 2.0f * arcTime;


    // Circle chord formula: http://mathworld.wolfram.com/CircularSegment.html
    float chordLen = 2.0f * radius * sin(0.5f * arcAngle_rad);
    // distance from center to chord line
    float r = 0.5f * sqrt(4 * radius*radius - chordLen*chordLen);
    // Chord unit vector
    Eigen::Vector3f chordUnitVec = radiusUnitVec.cross(upVec);
    chordUnitVec.normalize(); // In case the supplied normal is not normalized.
    // Intersection of line cntr<->pos with chord. 
    Eigen::Vector3f chordIntersec =  circleCntr + r * radiusUnitVec;    
    Eigen::Vector3f circleNormal = radiusUnitVec.cross(chordUnitVec);

    std::unique_ptr<MultiTrajectorySegment> multiTrajectory = std::make_unique<MultiTrajectorySegment>();

    Eigen::Vector3f start = chordIntersec - 0.5f * chordLen * chordUnitVec;
    Eigen::Vector3f end = chordIntersec + 0.5f * chordLen * chordUnitVec;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory1 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, fullCycleTime * stepFrac, velStep, velStep);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory1));

    start = end;
    end -= circleNormal * lift;
    Eigen::Vector3f normal = circleCntr - start;
    normal.normalize();
    std::unique_ptr<TrajectorySegment> cirldeTrajectory2 = std::make_unique<CircularTrajectorySegment>(start, end, normal, 0.5001f * lift, arcTime, velStep, velRetract);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory2));

    start = end;
    end -= chordLen * chordUnitVec;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory3 = std::make_unique<CircularTrajectorySegment>(start, end, -circleNormal, radius, retractTime, velStep, velStep);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory3));

    start = end;
    end += circleNormal * lift;
    normal = circleCntr - end;
    normal.normalize();
    std::unique_ptr<TrajectorySegment> cirldeTrajectory4 = std::make_unique<CircularTrajectorySegment>(start, end, normal, 0.5001f * lift, arcTime, velRetract, velStep);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory4));

    return std::move(multiTrajectory);
}

std::unique_ptr<TrajectorySegment> CreateLinearStepTrajectory(const Eigen::Vector3f& step, const Eigen::Vector3f& pos, float lift, float fullCycleTime)
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
    // The fullCycleTime is divided in four so the step time is 3/4 and 1/4 is remaing part.
    // The velocity is stepLen/(fullCycleTime * 3/4 ) 
    //

    Eigen::Vector3f circleNormal = step.cross(Eigen::Vector3f::UnitZ());
    circleNormal.normalize();

    float radius = 0.51f * lift;
    float arcLen = (float)M_PI * lift; // Length of both arc segments.
    float stepLen = step.norm();
    float retractFrac = 0.25f;
    float stepFrac = 0.75f;
    float velStep = stepLen / (stepFrac * fullCycleTime); // Constant velocity of step.
    float velRetract = (stepLen + arcLen) / (retractFrac * fullCycleTime); // Avarage velocity of retraction.
    float arcTime = retractFrac * fullCycleTime * 0.5f * arcLen / (stepLen + arcLen);
    float retractTime = retractFrac * fullCycleTime - 2.0f * arcTime;
    std::unique_ptr<MultiTrajectorySegment> multiTrajectory = std::make_unique<MultiTrajectorySegment>();

    Eigen::Vector3f start = pos - step * 0.5f;
    Eigen::Vector3f end = pos + step * 0.5f;
    std::unique_ptr<TrajectorySegment> lineTrajectory1 = std::make_unique<LinearTrajectorySegment>(start, end, fullCycleTime * stepFrac, velStep, velStep);
    multiTrajectory->AddSegment(std::move(lineTrajectory1));

    start = end;
    end = end + Eigen::Vector3f::UnitZ() * lift;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory2 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, arcTime, velStep, velRetract);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory2));

    start = end;
    end = end - step;
    std::unique_ptr<TrajectorySegment> lineTrajectory3 = std::make_unique<LinearTrajectorySegment>(start, end, retractTime, velRetract, velRetract);
    multiTrajectory->AddSegment(std::move(lineTrajectory3));

    start = end;
    end = end - Eigen::Vector3f::UnitZ() * lift;
    std::unique_ptr<TrajectorySegment> cirldeTrajectory4 = std::make_unique<CircularTrajectorySegment>(start, end, circleNormal, radius, arcTime, velRetract, velStep);
    multiTrajectory->AddSegment(std::move(cirldeTrajectory4));

    return std::move(multiTrajectory);
}

InsectModule::InsectModule()
{
}

InsectModule::~InsectModule()
{
}

void InsectModule::CreateNodes(BaseNode& rootNode)
{
    pNodeInsect = rootNode.FindOrCreateChild<BaseNode>("Insect");
    pNodeInsect->Subscribe(this);

    pNodeLegs = pNodeInsect->FindOrCreateChild<BaseNode>("Legs");

    pNodeEnable = pNodeInsect->FindOrCreateChild<BoolNode>("Enable", false);
    pNodeSoftStart = pNodeInsect->FindOrCreateChild<BoolNode>("SoftStart", false);    
    pNodeTestTrajectory = pNodeInsect->FindOrCreateChild<BoolNode>("TestTrajectory", false);

    pNodePosition = pNodeInsect->FindOrCreateChild<Pos3D_32f_Node>("BodyPosition");
    for(size_t legIdx = 0; legIdx < legs.size(); ++legIdx)
    {
        legs[legIdx].CreateNodes(*pNodeLegs, legIdx);
    }
}
void InsectModule::LookupNodes()
{
    pNodeShutdown = pNodeInsect->FindNode<BoolNode>("/SystemInfo/ShutdownSystem", false);
    if (pNodeShutdown)
        pNodeShutdown->Subscribe(this);

    // Create test trajectory.
    float stepDuration = 180.0f;
    for (auto& leg : legs)
    {
        Eigen::Vector3f pos(0.12f, 0.0f, -0.13f);
        Eigen::Vector3f step(0.08f, 0.08f, 0.0f);
        float dir = 1.0f;
        if (leg.GetLegID() & 0x1) // Left side move the other way.
        {
            dir *= -1.0f;
            step *= -1.0f;
        }

        Eigen::Vector3f center(0.0f, 0.0f, -0.13f);
        std::unique_ptr<TrajectorySegment> traj = CreateCircularStepTrajectory(center, pos, dir * Eigen::Vector3f::UnitZ(), 1.0f, dir * 0.02f, stepDuration);
            //CreateLinearStepTrajectory(step, pos, 0.015f, stepDuration);
        leg.SetTrajectory(std::move(traj));
    }

    legs[0].RestartTrajectory(stepDuration * 0.00f);
    legs[1].RestartTrajectory(stepDuration * 0.25f);
    legs[4].RestartTrajectory(stepDuration * 0.50f);
    legs[5].RestartTrajectory(stepDuration * 0.75f);

}

void InsectModule::Notify()
{
    if (pNodeEnable->IsValueChanged())
    {
        EnableLegs(pNodeEnable->Get());
    }
    if (pNodeSoftStart->IsValueChanged() && pNodeSoftStart->Get() && !isSoftStarting)
    {
        if (pNodeEnable->Get())
        {   // Disable softstart when already enabled.
            pNodeSoftStart->Set(false);
        }
        else
        {
            SoftStart();
        }
    }
    if (pNodeShutdown->Get()) // When shutting down - disable all legs
    {
        EnableLegs(false);
    }
}

void InsectModule::Execute()
{
    for (auto& leg : legs)
    {
        if (pNodeTestTrajectory->Get())
        {
            if (!leg.UpdateTrajectory(1.0f))
            {
                leg.RestartTrajectory();
            }
        }
        //leg.Execute();
        //leg.SetGoal();
    }
}

void InsectModule::OnTimer()
{
    if (isSoftStarting)
    {
        SoftStart();
    }
}

void InsectModule::EnableLegs(bool enable)
{
    for (auto& leg : legs)
    {
        leg.EnableLeg(enable);
    }
    pNodeEnable->Set(enable);
}

void InsectModule::SoftStart()
{
    if (!isSoftStarting)
    {   // Initialize softstart sequence.
        isSoftStarting = true;
        SetTimeout(500);
        pNodeSoftStart->Set(true);
        initLeg = 0;
    }

    if (initLeg >= numLegs)
    {   // Finalize softstart sequence.
        isSoftStarting = false;
        SetTimeout(NoTimeOut);
        pNodeEnable->Set(true);
        pNodeSoftStart->Set(false);
        return;
    }
    // Perform softstart step.
    legs[initLeg].EnableLeg(true);
    initLeg++;
}
