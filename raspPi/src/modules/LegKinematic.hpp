#pragma once

#include <iostream>
#include <Eigen/Dense>

#define _USE_MATH_DEFINES
#include <math.h>

#define RAD2DEG(rad)  ((rad) * 180.0f / float(M_PI))
#define DEG2RAD(deg)  ((deg) * float(M_PI) / 180.0f)

class LegKinematic
{
public:
    LegKinematic();
    ~LegKinematic();
    void setGoal(float goalX, float goalY, float goalZ);
    void getGoal(float& goalX, float& goalY, float& goalZ);
    void setJoints(float joint1Angle_deg, float joint2Angle_deg, float joint3Angle_deg);
    float getJoint(int idx) const;
    void getJoints(float& joint1Angle_deg, float& joint2Angle_deg, float& joint3Angle_deg) const;
    void SetDH_LinkDist(int idx, float linkDist);
    void SetDH_LinkDist(float linkDist1, float linkDist2, float linkDist3)
    {
        m_ld[0] = linkDist1; m_ld[1] = linkDist3; m_ld[2] = linkDist3;
    }

    void SetDH_LinkAngle(int idx, float linkAngle);
    void SetDH_LinkAngle(float linkAngle1, float linkAngle2, float linkAngle3)
    {
        m_la[0] = linkAngle1; m_la[1] = linkAngle2; m_la[2] = linkAngle3;
    }


private:
    void jointLinkTransform(Eigen::Matrix4f &jointLinkT, float jd, float ja, float ld, float la) const;
    void jointTransform(Eigen::Matrix4f &jointT, float jd, float ja) const;
    void linkTransform(Eigen::Matrix4f &linkT, float ld, float la) const;
    void invertTransform(Eigen::Matrix4f &transform) const;

    /*
    Denavit-Hartenberg description:

    Jd - Joint distance
    Ja - Joint angle
    La - link angle
    Ld - link distance

    _____________
    Joint   jd  Ja  La  Ld
    0       0   a1  90  d1
    1       0   a2  0   d2
    2       0   a3  0   d3
    */

    float m_jd[3];
    float m_ja[3];
    float m_ld[3];
    float m_la[3];
};


