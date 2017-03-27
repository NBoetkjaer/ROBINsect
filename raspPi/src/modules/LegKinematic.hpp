#pragma once

#include <iostream>
#include <Eigen/Dense>

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
private:
    void jointLinkTransform(Eigen::Matrix4f &jointLinkT, float jd, float ja, float ld, float la) const;
    void jointTransform(Eigen::Matrix4f &jointT, float jd, float ja) const;
    void linkTransform(Eigen::Matrix4f &linkT, float ld, float la) const;
    void invertTransform(Eigen::Matrix4f &transform) const;
    float m_joint1Angle_rad, m_joint2Angle_rad, m_joint3Angle_rad;
    float d1, d2, d3; // mm
};


