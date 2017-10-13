#pragma once
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>

// Abstract base class for trajectory segments.
class TrajectorySegment
{
public:
    TrajectorySegment() = delete; // No default constructor.
    TrajectorySegment(float duration):
        durationTime(duration)
    {};
    virtual ~TrajectorySegment() {};
    float GetDeltaTime() const { return durationTime; }
    virtual void GetPosition(float time, Eigen::Vector3f& position) const = 0;
protected:
    float durationTime;
};

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

class LinearTrajectorySegment :public SimpleTrajectorySegment
{
public:
    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration):
        SimpleTrajectorySegment(duration), startPos(start)
    {
        slope = (target - startPos) / durationTime;
    };

    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration, float startVelocity, float endVelocity) :
        SimpleTrajectorySegment(duration), startPos(start)
    {
        slope = (target - startPos) / durationTime;
        UseSpline(startVelocity, endVelocity);
    };

    virtual ~LinearTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        if (bSpline)
        {
            time = GetSplineTime(time);
        }
        position = startPos + slope * time;
    };
protected:
    virtual float GetCurveLength() const override
    {
        return (slope*durationTime).norm();
    };
    Eigen::Vector3f startPos;
    Eigen::Vector3f slope;
};

class CircularTrajectorySegment :public SimpleTrajectorySegment
{
public:
    CircularTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
        const Eigen::Vector3f& normal, float radius, float duration)
        :SimpleTrajectorySegment(duration)
    {
        Initialize(start, target, normal, radius);
    }

    CircularTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
        const Eigen::Vector3f& normal, float radius, float duration, float startVelocity, float endVelocity)
        : SimpleTrajectorySegment(duration)
    {
        Initialize(start, target, normal, radius);
        UseSpline(startVelocity, endVelocity);
    }

    virtual ~CircularTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        if (bSpline)
        {
            time = GetSplineTime(time);
        }

        // Calculate position along the arc.
        float angle_rad = arcAngle_rad * time / durationTime;
        Eigen::Vector3f pos(arcRadius * cos(angle_rad), arcRadius * sin(angle_rad), 0.0f);
        position = fromCirclePlane * pos;
    }
protected:
    virtual float GetCurveLength() const override
    {
        return abs(arcAngle_rad * arcRadius);
    };
    Eigen::AffineCompact3f fromCirclePlane;
    float arcAngle_rad;
    float arcRadius;
private:
    void Initialize(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
        const Eigen::Vector3f& normal, float radius)
    {
        // A negative radius means the circular path is followed in a clockwise rotation around the normal.
        // A positive radius is a counter clockwise path.
        /////////////////////////////////////////
        // Temporarily construct a transformation matrix to a coordinate system where the circle trajectory is in the x-y plane
        // and start is located in the origin.
        Eigen::Vector3f xAxis = target - start;
        float chordLength = xAxis.norm();
        if (chordLength > 2.0f * abs(radius) || chordLength == 0.0f) return;
        // Find y-axis so that we have a right hand coordinate system.
        Eigen::Vector3f zAxis = normal;
        xAxis.normalize();
        zAxis.normalize();
        Eigen::Vector3f yAxis = zAxis.cross(xAxis);
        // yAxis.normalize(); Not necessary since x- and z-axis is normalized prior to cross product.
        fromCirclePlane.setIdentity();
        fromCirclePlane.matrix().col(0) = xAxis;
        fromCirclePlane.matrix().col(1) = yAxis;
        fromCirclePlane.matrix().col(2) = zAxis;
        fromCirclePlane.matrix().col(3) = start;
        // Transform start and target to new coordinate system. Problem is now reduced to 2D
        // Start position is in the origin
        Eigen::AffineCompact3f toCirclePlane = fromCirclePlane;
        invertTransform(toCirclePlane);
        Eigen::Vector2f target2D = (toCirclePlane * target).head<2>();
        // Find circle center.
        // A line through the midpoint and perpendicular to the line segment (start, target) will also pass through the center.
        // since target is also located on the positive x-axis (and start is the origin) x coordinate of center is midway to target.
        // y is given by Pythagoras. Actually two circles exist, Here we use the circle with a positive y center. To get the other circle invert the normal vector.
        float cntrX = chordLength / 2.0f;
        float cntrY = sqrt(radius*radius - cntrX *cntrX);
        Eigen::Vector3f center(cntrX, cntrY, 0);
        // Transform center back to original coordinate system.
        center = fromCirclePlane * center;
        /////////////////////////////////////////
        // Construct final transformation that brings the circle center to the origin and put start on the positive x-axis.
        // circle is in the x-y plane.
        xAxis = (start - center);
        xAxis.normalize();
        yAxis = zAxis.cross(xAxis);
        fromCirclePlane.matrix().col(0) = xAxis;
        fromCirclePlane.matrix().col(1) = yAxis;
        fromCirclePlane.matrix().col(3) = center;
        // Calculate the angle sweep.
        if (radius > 0)
        {   // Counter clockwise orientation around normal.
            arcAngle_rad = 2.0f * asin(chordLength / (2.0f * radius)); // arcAngle_rad is positive.
        }
        else
        {   // Clockwise orientation around normal ~ the longest way around in the circle.
            arcAngle_rad = (float)(-2.0 * M_PI) - 2.0f * asin(chordLength / (2.0f * radius)); // arcAngle_rad is negative.
        }
        arcRadius = abs(radius);
    };

    void invertTransform(Eigen::AffineCompact3f &transform) const
    {
        // Invert a pure rotate and translate matrix.
        transform.matrix().topLeftCorner<3, 3>().transposeInPlace();
        transform.matrix().topRightCorner<3, 1>() = -1.0f * transform.matrix().topLeftCorner<3, 3>()
                                                    * transform.matrix().topRightCorner<3, 1>();
    }
};