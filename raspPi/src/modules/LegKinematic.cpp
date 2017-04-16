#include "LegKinematic.hpp"
#include <iostream>

using namespace std;
using namespace Eigen;

LegKinematic::LegKinematic() :
    m_jd{ 0 },
    m_ja{ 0 },
    m_ld{ 0.090f, 0.125f, 0.150f },
    m_la{ DEG2RAD(90.0f), 0.0f, 0.0f }
{
}

LegKinematic::~LegKinematic()
{
}

void LegKinematic::setGoal(float goalX, float goalY, float goalZ)
{
    // Solve the inverse kinematic for the 3 DOF leg.

    float jointAngles[3];
    // Joint 1 angle is given by:
    jointAngles[0] = atan2(goalY, goalX);
    // Find the end of link 1.
    // Matrix4f jointT; // Joint  transform.
    // jointTransform(jointT, 0.0f, m_joint1Angle_rad);
    // Matrix4f linkT; // Link transform.
    // linkTransform(linkT, ld[0], M_PI/2.0f);	
    Matrix4f jointLinkT; // = jointT * linkT;
    jointLinkTransform(jointLinkT, m_jd[0], jointAngles[0], m_ld[0], m_la[0]);

    invertTransform(jointLinkT);
    // Transform goal position into end of link 1 franejoint 1 frame.
    Vector4f goal;
    goal << goalX, goalY, goalZ, 1.0f;
    goal = jointLinkT * goal;

    // The transformed goal point have a z value of zero.
    // Due to the geometry of the 3 DOF leg, the problem is now reduced to finding the intersection of two circles.
    // One is centered in (0,0) end of Link 1  the other centered around the transformed goal point.

    // http://paulbourke.net/geometry/circlesphere/
    float x, y, x2, y2, x3, y3;
    x = goal(0);
    y = goal(1);
    float dist = hypot(x, y);
    if (dist > (m_ld[1] + m_ld[2]) || dist < fabs(m_ld[1] - m_ld[2]) || dist == 0.0f)
    {
        cout << "Goal un-reachable.";
        return;
    }

    float a = ((m_ld[1]*m_ld[1]) - (m_ld[2]*m_ld[2]) + (dist*dist)) / (2.0f*dist);
    if (m_ld[1] == a)
    {
        cout << "Goal has only one solution.";
    }
    float h = sqrtf(m_ld[1]*m_ld[1] - a*a);
    x2 = x * a / dist;
    y2 = y * a / dist;


    // Joint 2 angle is given by:
    // x3 = x2 + h * y / dist;
    // y3 = y2 - h * x / dist;
    // jointAngles[1] = atan2(y3, x3);
    // Or tbe other solution ...
    x3 = x2 - h * y / dist;
    y3 = y2 + h * x / dist;
    jointAngles[1] = atan2(y3, x3);
    //cout << "m_joint2Angle_rad: " << RAD2DEG(m_joint2Angle_rad) << std::endl;	

    // Transform the goal point into the coordinate frame of link 2.
    jointLinkTransform(jointLinkT, m_jd[1], jointAngles[1], m_ld[1], m_la[1]);
    invertTransform(jointLinkT);
    goal = jointLinkT * goal;

    // x = x - x3;
    // y = y - y3;
    // float s = sin(-jointAngles[1]);
    // float c = cos(-jointAngles[1]);
    // x = x*c - y*s;
    // y = x*s + y*c;
    jointAngles[2] = atan2(goal(1), goal(0));

    m_ja[0] = jointAngles[0];
    m_ja[1] = jointAngles[1];
    m_ja[2] = jointAngles[2];
}

void LegKinematic::getGoal(float& goalX, float& goalY, float& goalZ)
{
    Matrix4f transform = Matrix4f::Identity();
    for (int i = 0; i < 3; ++i)
    {
        Matrix4f jointLinkT;
        jointLinkTransform(jointLinkT, m_jd[i], m_ja[i], m_ld[i], m_la[i]);
        transform = transform * jointLinkT;
        /* Debug output.
        std::cout << "Link" << i << std::endl << std::endl;
        std::cout << "Joint" << std::endl << jointT << std::endl;
        std::cout << "Link" << std::endl << linkT << std::endl;
        std::cout << "Transform" << std::endl << transform << std::endl << std::endl;
        */

    }
    goalX = transform(0, 3);
    goalY = transform(1, 3);
    goalZ = transform(2, 3);
}

void LegKinematic::setJoints(float joint1Angle_deg, float joint2Angle_deg, float joint3Angle_deg)
{
    m_ja[0] = DEG2RAD(joint1Angle_deg);
    m_ja[1] = DEG2RAD(joint2Angle_deg);
    m_ja[2] = DEG2RAD(joint3Angle_deg);
}

void LegKinematic::getJoints(float& joint1Angle_deg, float& joint2Angle_deg, float& joint3Angle_deg) const
{
    joint1Angle_deg = RAD2DEG(m_ja[0]);
    joint2Angle_deg = RAD2DEG(m_ja[1]);
    joint3Angle_deg = RAD2DEG(m_ja[2]);
}
float LegKinematic::getJoint(int idx) const
{
    if (idx < 0 || idx > 2)
    {
        return 0.0f;
    }
    return RAD2DEG(m_ja[idx]);

}
void LegKinematic::SetDH_LinkDist(int idx, float linkDist)
{
    if (idx < 0 || idx > 2)
    {
        return;
    }
    m_ld[idx] = linkDist;
}

void LegKinematic::SetDH_LinkAngle(int idx, float linkAngle)
{
    if (idx < 0 || idx > 2)
    {
        return;
    }
    m_la[idx] = linkAngle;
}

void LegKinematic::jointTransform(Matrix4f &jointT, float jd, float ja) const
{
    float s = sin(ja);
    float c = cos(ja);
    jointT.setIdentity();
    jointT(0, 0) = c;
    jointT(1, 1) = c;
    jointT(0, 1) = -s;
    jointT(1, 0) = s;
    jointT(2, 3) = jd;
}
void LegKinematic::linkTransform(Matrix4f &linkT, float ld, float la) const
{
    const float s = sin(la);
    const float c = cos(la);
    linkT.setIdentity();
    linkT(1, 1) = c;
    linkT(2, 2) = c;
    linkT(1, 2) = -s;
    linkT(2, 1) = s;
    linkT(0, 3) = ld;
}
void LegKinematic::jointLinkTransform(Eigen::Matrix4f &jointLinkT, float jd, float ja, float ld, float la) const 
{
    // Joint  transform.
    jointTransform(jointLinkT, jd, ja);
    Matrix4f linkT; // Link transform.
    linkTransform(linkT, ld, la);
    // Combined transform.
    jointLinkT = jointLinkT * linkT;
}

void LegKinematic::invertTransform(Matrix4f &transform) const
{
    transform.topLeftCorner<3, 3>().transposeInPlace();
    transform.topRightCorner<3, 1>() = -1.0f * transform.topLeftCorner<3, 3>() * transform.topRightCorner<3, 1>();
}


