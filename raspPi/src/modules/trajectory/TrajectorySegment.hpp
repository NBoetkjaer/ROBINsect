#pragma once
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>

// Abstract base class for all trajectory segments.
class TrajectorySegment
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    TrajectorySegment() = delete; // No default constructor.
    TrajectorySegment(float duration):
        durationTime(duration)
    {};
    virtual ~TrajectorySegment() {};
    inline float GetDuration() const { return durationTime; }
    virtual void GetPosition(float time, Eigen::Vector3f& position) const = 0;
protected:
    float durationTime;
};

// Abstract base class for a trajectory, where the traveled distance is a linear function of time.
// GetPosition will return equidistantly spaced points for times that are also equidistantly spaced.
class SimpleTrajectorySegment : public TrajectorySegment
{
private:
    float m0, m1;
public:
    SimpleTrajectorySegment(float duration) : TrajectorySegment(duration) {};
    virtual ~SimpleTrajectorySegment() {};
protected:
    bool bSpline = false;
    virtual float GetCurveLength() const = 0;
    void UseSpline(float startVelocity, float endVelocity)
    {
        bSpline = true;
        // Determine the Hermite spline coefficients.
        //
        // p0 = start time = 0;
        // p1 = end time = durationTime;
        float curveLength = GetCurveLength();
        // Start velocity 
        m0 = startVelocity * durationTime * durationTime / curveLength; // Normalize by segment length (assuming GetPosition() will return equidistantly spaced positions as a funtion of time)
        // End velocity
        m1 = endVelocity * durationTime * durationTime / curveLength;   // Normalize by segment length .
    };

    float GetSplineTime(float time) const
    {
        // Interpolate the time by a Hermite spline.
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline
        // Hermite interpolation coefficients.

        // h00 = 2t^3 - 3t^2 + 1;
        // h10 = t^3 - 2t^2 + t;
        // h01 = -2t^3 + 3t^2;	
        // h11 = t^3 - t^2;

        // interpolated time: t = [0:1] normalized time ~ [0:durationTime].
        // f(t) = h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1. // Note: p0 is always zero and p1 equals the durationTime.
        float time1 = time / durationTime;
        float time2 = time1*time1;
        float time3 = time1*time1*time1;
        return m0 * (time3 - 2.0f * time2 + time1) + durationTime * (-2.0f * time3 + 3.0f * time2) + m1 * (time3 - time2);
    }
};
