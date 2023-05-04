#pragma once
#include "TrajectorySegment.hpp"

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
    virtual void GetPosition(float time, Eigen::Vector3f& position) const override;
protected:

    virtual float GetCurveLength() const override
    {
        return abs(m_arcAngle_rad * m_arcRadius);
    };

    Eigen::AffineCompact3f fromCirclePlane;
    float m_arcAngle_rad;
    float m_arcRadius;

private:
    void Initialize(const Eigen::Vector3f& start, const Eigen::Vector3f& target,
        const Eigen::Vector3f& normal, float radius);

    void invertTransform(Eigen::AffineCompact3f &transform) const
    {
        // Invert a pure rotate and translate matrix.
        transform.matrix().topLeftCorner<3, 3>().transposeInPlace();
        transform.matrix().topRightCorner<3, 1>() = -1.0f * transform.matrix().topLeftCorner<3, 3>()
                                                    * transform.matrix().topRightCorner<3, 1>();
    }
};