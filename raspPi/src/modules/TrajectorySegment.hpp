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

class LinearTrajectorySegment :public TrajectorySegment
{
public:
    LinearTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target, float duration):
        TrajectorySegment(duration), startPos(start)
    {
        slope = (target - startPos) / durationTime;
    };
    virtual ~LinearTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        position = startPos + slope * time;
    }
protected:
    Eigen::Vector3f startPos;
    Eigen::Vector3f slope;
};

class CircularTrajectorySegment :public TrajectorySegment
{
public:
    CircularTrajectorySegment(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
                              const Eigen::Vector3f& normal, float radius, float duration):
        TrajectorySegment(duration), arcRadius(radius)
    {
        // A negative radius means the circular path is followed in a clockwise rotation around the normal.
        // A positive radius is a counter clockwise path.
        /////////////////////////////////////////
        // Temporarily construct a transformation matrix to a coordinate system where the circle trajectory is in the x-y plane
        // and start is located in the origin.
        Eigen::Vector3f xAxis = target - start;
        float chordLength = xAxis.norm();
        if (chordLength > 2.0f * radius) return;
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
        // since target is also located on the positive x-axis (and start is the origin) x coord of center is midway to target.
        // y is given by pythagoras.
        float cntrX = chordLength / 2.0f;
        float cntrY = copysignf(sqrt(radius*radius - cntrX *cntrX), radius); // Make cntrY take same sign as radius.
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

        arcAngle_rad = 2.0f * asin(chordLength / (2.0f * radius));
        if (arcAngle_rad < 0) // ~ radius must be negative (== a clockwise orientation around normal)
        {   // Take the long pat 
            arcAngle_rad += (float)M_2_PI;
        }

        toCirclePlane = fromCirclePlane;
        invertTransform(toCirclePlane);
        std::cout << toCirclePlane.matrix() << std::endl;

        Eigen::Vector3f testVec;
        testVec = toCirclePlane * center;
        std::cout << "center: " << testVec.transpose() << std::endl;
        testVec = toCirclePlane * start;
        std::cout << "start: " << testVec.transpose() << std::endl;
        testVec = toCirclePlane * target;
        std::cout << "target: " << testVec.transpose() << std::endl;
    };

    virtual ~CircularTrajectorySegment() {};
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override
    {
        // Calculate position along the arc.
        float angle_rad = arcAngle_rad * time / durationTime;
        Eigen::Vector3f pos(arcRadius * cos(angle_rad), arcRadius * sin(angle_rad), 0.0f);
        position = fromCirclePlane * pos;
   }
protected:
    Eigen::AffineCompact3f fromCirclePlane;
    float arcAngle_rad;
    float arcRadius;
private:
    void invertTransform(Eigen::AffineCompact3f &transform) const
    {
        // Invert a pure rotat and translate matrix.
        transform.matrix().topLeftCorner<3, 3>().transposeInPlace();
        transform.matrix().topRightCorner<3, 1>() = -1.0f * transform.matrix().topLeftCorner<3, 3>()
                                                    * transform.matrix().topRightCorner<3, 1>();
    }
};