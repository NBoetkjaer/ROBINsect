#include "CircularTrajectorySegment.hpp"

void CircularTrajectorySegment::Initialize(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
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
}

void CircularTrajectorySegment::GetPosition(float time, Eigen::Vector3f& position) const
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