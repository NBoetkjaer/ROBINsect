#pragma once
#include "../trajectory/TrajectorySegment.hpp"
#include "../constants.hpp"
#include "../geometry/plane3d.hpp"

#include <Eigen/Dense>

#include <array>
#include <memory>


// Gait states: 
//  stopped (default state)
//      setup initial leg positions
//  Initializing 
//      Drive leg positions into the gait sequense.
//  Initialized 
//      all legs are ready.
//  Active
//      Keep running the gait sequence (while changing direction and speed as requested)
//  stopping
//      Slow down and bring all legs to the ground.

enum class GaitStates
{
    Stopped,
    Initializing, 
//    Initialized,
    Active,
    Stopping,
};


class Gait
{
public:
    Gait() = default;

    virtual ~Gait() {};

    virtual void UpdateGait(float elapsedTime_s) = 0;

    GaitStates GetState() const { return m_state; }

    void SetLegRange(int legId, const Eigen::Vector3f &cntrPos, float range);
    void GetLegRange(int legId, Eigen::Vector3f& cntrPos, float &range) const;
    
    void GetLegPosition(int legId, Eigen::Vector3f& pos) const;
    void SetLegPosition(int legId, const Eigen::Vector3f& pos);

    void SetStepHeight(float stepHeight) { m_stepHeight = stepHeight; }
    float GetStepHeight() const { return m_stepHeight; }

    void SetStepLength(float stepLength) { m_stepLength = stepLength; }
    float GetStepLenght() const { return m_stepLength; }

    void SetStepDirection(float direction_rad) { m_stepDir_rad = direction_rad; }
    float GetStepDirection() const { return m_stepDir_rad; }

    void SetVelocity(float velocity) { m_velocity = velocity; }
    float GetVelocity() const { return m_velocity; }

    void SetAngularVelocity(float angularVelocity_rad) { m_angularVelocity_rad = angularVelocity_rad; }
    float GetAngularVelocity() const { return m_angularVelocity_rad; }

    // ---------
    // Steering commands
    virtual void Reset() = 0;
    void Start() { Reset(); SetState(GaitStates::Initializing); }
    void Stop() { SetState(GaitStates::Stopped); }
    
protected:

    bool SetState(GaitStates newState);
    GaitStates m_state = GaitStates::Stopped;
    // The insect body position is (0,0,0) and y-axis is pointing forward, z-axis is pointing up.
    // Define the leg range limits. Here it is a sphere. ToDo make a LegRange class to support other geometeries than spheres.
    std::array< Eigen::Vector3f, NUM_LEGS> m_legCentralPos;
    std::array<float, NUM_LEGS> m_legRange = { 0, };
    // Initial leg positions
    std::array< Eigen::Vector3f, NUM_LEGS> m_legPos;
    // Step center pos
    std::array< Eigen::Vector3f, NUM_LEGS> m_stepCntr;
    // Define the ground plane    
    Geometry3D::Plane3D_32f m_groundPlane = Geometry3D::Plane3D_32f( { 0.0f, 0.0f, 1.0f } , -0.1f );
    // Define step    
    float m_maxLegVelocity = 0.20f; // m/s
    float m_stepHeight = 0.015f; // The step height in meters.
    float m_stepDir_rad = 0.0f;  // Direction of walking in radians. zero equals straight forward.
    float m_velocity = -0.02f;     // Walking velocity in m/s.
    float m_angularVelocity_rad = 0.01f; // Turning speed in radians per seconds.
    float m_stepLength = 0.04f;          // The requested step length in meters

    Eigen::Vector3f m_stepVector;
    float m_stepDuration_s = 0.0f;
    float m_deltaTime_s = 0.0f;

public:

    // Utillity functions used to generate the step m_trajectories.
    static std::unique_ptr<TrajectorySegment> CreateCircularStepTrajectory(const Eigen::Vector3f& circleCntr, const Eigen::Vector3f& pos, const Eigen::Vector3f& upVec, float arcAngle_rad, float lift, float fullCycleTime, float stepCycleRatio = 0.75f);
    static std::unique_ptr<TrajectorySegment> CreateLinearStepTrajectory(const Eigen::Vector3f& step, const Eigen::Vector3f& pos, float lift, float fullCycleTime, float stepCycleRatio = 0.75f);
};

