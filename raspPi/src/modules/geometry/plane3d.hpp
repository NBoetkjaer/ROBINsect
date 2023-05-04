#include  <Eigen/Dense>

#include "line3d.hpp"

namespace Geometry3D
{
	template<typename T = float>
	class Plane3D
	{
		using Point3D = Eigen::Matrix < T, 3, 1>;
        // Hessian Normal form description of the plane
        Point3D m_planeNormal; // Must have unit length.
        T m_distanceFromOrigo;

    public:
        Plane3D()
        {
            m_planeNormal = Point3D(1.0, 0.0, 0.0);
            m_distanceFromOrigo = 0;
        }

        Plane3D(const Point3D& planeNormal, T distanceFromOrigo)
        {
            // Make the normal vector a unit vector
            m_planeNormal = planeNormal / planeNormal.norm();
            m_distanceFromOrigo = distanceFromOrigo;
        }

        Plane3D(const Point3D& planeNormal, const Point3D& pointOnPlane)
        {
            // Make the normal vector a unit vector
            m_planeNormal = planeNormal / planeNormal.norm();
            m_distanceFromOrigo = -(m_planeNormal.dot( pointOnPlane));
        }

        inline Point3D Normal() const { return m_planeNormal; }        
        inline T DistanceFromOrigo()const { return m_distanceFromOrigo; }
        inline Point3D PointOnPlane() const { return m_distanceFromOrigo * m_planeNormal; }
        
        // Calculates the perpendicular distance from a point to 'this' plane.
        // and also gives the closest point on 'this' plane.
        T DistanceToPoint(const Point3D& point, Point3D& closestPointOnPlane) const
        {
            // Note that the m_planeNormal must be a unit vector.
            auto dist = m_planeNormal.dot(point) + m_distanceFromOrigo;
            closestPointOnPlane = point - m_planeNormal*dist;
            return dist;
        }

        // Calculates the perpendicular distance from a point to 'this' plane.
        T DistanceToPoint(const Point3D& point) const
        {
            // Note that the m_planeNormal must be a unit vector.
            return m_planeNormal.dot(point) + m_distanceFromOrigo;
        }

        // Returns a vector in the plane that is a projection of the given vector.
        Point3D ProjectVectorOntoPlane(const Point3D& vec)
        {
            return vec - (vec.dot(m_planeNormal) / m_planeNormal.dot(m_planeNormal)) * m_planeNormal;
        }

        bool IntersectionWithLine(const Line3D<T>& intersectingLine, Point3D& intersectionPoint) const
        {
            // See: http://paulbourke.net/geometry/pointlineplane/
            T denominator = m_planeNormal.dot(intersectingLine.Direction());
            if (denominator == 0.0)
            {
                return false; // Line and plane are parallel
            }
            T u = m_planeNormal.dot(m_planeNormal * (-m_distanceFromOrigo) - intersectingLine.PointOnLine()) / denominator;
            intersectionPoint = intersectingLine.PointOnLine() + intersectingLine.Direction()* u;

            return true;
        }

        bool IntersectionWithLineSegment(const Point3D& lineStart, const Point3D& lineEnd, Point3D& intersectionPoint) const
        {
            // See: http://paulbourke.net/geometry/pointlineplane/
            float denominator = m_planeNormal.dot(lineEnd - lineStart);
            if (denominator == 0.0)
            {
                return false; // Line and plane are parallel
            }
            float u = m_planeNormal.dot(m_planeNormal * (-m_distanceFromOrigo) - lineStart) / denominator;

            if (u >= 0.0 && u <= 1.0)
            {
                intersectionPoint = lineStart + (lineEnd - lineStart) * u;
                return true;
            }
            return false;
        }
	};

    using Plane3D_32f = Plane3D<float>;
    using Plane3D_64f = Plane3D<double>;
}