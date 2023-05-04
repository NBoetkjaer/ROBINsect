#include  <Eigen/Dense>

namespace Geometry3D
{
	template<typename T>
	class Line3D
	{    
		using Point3D = Eigen::Matrix < T, 3, 1>;
        Point3D m_directionOfLine;
        Point3D m_pointOnLine;
    public:
        Line3D()
        {
            m_directionOfLine = Point3D(1.0, 0.0, 0.0);
            m_pointOnLine = Point3D(0.0, 0.0, 0.0);
        }

        Line3D(Point3D directionOfLine, Point3D pointOnLine)
        {
            // Make the direction vector a unit vector
            m_directionOfLine = directionOfLine / directionOfLine.norm();
            m_pointOnLine = pointOnLine;
        }

        inline Point3D Direction() const { return m_directionOfLine; }
        inline Point3D PointOnLine()const { return m_pointOnLine; }


        // Calculates the perpendicular distance from a point to 'this' line.
        // and also gives the closest point on 'this' line.
        T DistanceToPoint(const Point3D& point, Point3D& closestPointOnLine) const
        {
            auto u = m_directionOfLine.dot(point - m_pointOnLine);
            closestPointOnLine = m_pointOnLine + m_directionOfLine * u;
            return (point - pointOnLine).norm();
        }

        // Calculates the perpendicular distance from a point to 'this' line.
        T DistanceToPoint(const Point3D& point) const
        {
            Point3D closestPointOnLine;
            return  DistanceToPoint(point, closestPointOnLine);
        }

        // Find the intersection of this line and a sphere with center in 'cntr' and 'radius'.
        // If no intersection exists, the function will return false.
        // If an intersection is found, the two intersections are returned in 'pt1' and 'pt2'.
        // 'pt1' may be equal to 'pt2' if the line is a tangent to the sphere.
        bool SphereIntersection(const Point3D& cntr, T radius, Point3D& pt1, Point3D&pt2) const
        {
            // http://paulbourke.net/geometry/circlesphere/
            T a = m_directionOfLine.dot(m_directionOfLine);
            T b = 2 * m_directionOfLine.dot(m_pointOnLine - cntr);
            T c = cntr.dot(cntr) + m_pointOnLine.dot(m_pointOnLine);
            c -= 2 * cntr.dot(m_pointOnLine);
            c -= radius * radius;
            T bb4ac = b * b - 4 * a * c;
            if( abs(a) < std::numeric_limits<T>::epsilon() || bb4ac < 0)
            {
                return false;
            }
            T mu1 = (-b + sqrt(bb4ac)) / (2 * a);
            T mu2 = (-b - sqrt(bb4ac)) / (2 * a);

            pt1 = m_pointOnLine + mu1 * m_directionOfLine;
            pt2 = m_pointOnLine + mu2 * m_directionOfLine;
            return true;
        }
	};

    using Line3D_32f = Line3D<float>;
    using Line3D_64f = Line3D<double>;
}
