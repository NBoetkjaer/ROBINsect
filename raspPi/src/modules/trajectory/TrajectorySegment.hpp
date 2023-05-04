#pragma once
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>

// Abstract base class for all trajectory segments.
class TrajectorySegment
{
public:
    TrajectorySegment() = delete; // No default constructor.
    TrajectorySegment(float duration):
        m_duration_s(duration)
    {};
    virtual ~TrajectorySegment() {};
    inline float GetDuration() const { return m_duration_s; }
    virtual void GetPosition(float time, Eigen::Vector3f& position) const = 0;
protected:
    float m_duration_s;
};

// Abstract base class for a trajectory, where the traveled distance is a linear function of time.
// GetPosition will return equidistantly spaced points for times that are also equidistantly spaced.
class SimpleTrajectorySegment : public TrajectorySegment
{
private:
    float m_m0, m_m1;
public:
    SimpleTrajectorySegment(float duration) : TrajectorySegment(duration) {};
    virtual ~SimpleTrajectorySegment() {};
protected:
    bool m_bSpline = false;
    virtual float GetCurveLength() const = 0;
    void UseSpline(float startVelocity, float endVelocity)
    {
        m_bSpline = true;
        // Determine the Hermite spline coefficients.
        //
        // p0 = start time = 0;
        // p1 = end time = m_duration_s;
        float curveLength = GetCurveLength();
        // Start velocity 
        m_m0 = startVelocity * m_duration_s * m_duration_s / curveLength; // Normalize by segment length (assuming GetPosition() will return equidistantly spaced positions as a funtion of time)
        // End velocity
        m_m1 = endVelocity * m_duration_s * m_duration_s / curveLength;   // Normalize by segment length .
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

        // interpolated time: t = [0:1] normalized time ~ [0:m_duration_s].
        // f(t) = h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1. // Note: p0 is always zero and p1 equals the m_duration_s.
        float time1 = time / m_duration_s;
        float time2 = time1*time1;
        float time3 = time1*time1*time1;
        return m_m0 * (time3 - 2.0f * time2 + time1) + m_duration_s * (-2.0f * time3 + 3.0f * time2) + m_m1 * (time3 - time2);
    }
};
