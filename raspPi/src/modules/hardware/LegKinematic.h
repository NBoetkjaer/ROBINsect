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
	float getJoint(int idx);
	void getJoints(float& joint1Angle_deg, float& joint2Angle_deg, float& joint3Angle_deg);
private:
	void jointLinkTransform(Eigen::Matrix4f &jointLinkT, float jd, float ja, float ld, float la);
	void jointTransform(Eigen::Matrix4f &jointT, float jd, float ja);
	void linkTransform(Eigen::Matrix4f &linkT, float ld, float la);
	void invertTransform(Eigen::Matrix4f &transform);
	float m_joint1Angle_rad,  m_joint2Angle_rad,  m_joint3Angle_rad;	
	float d1, d2, d3; // mm
	float test[2];
	
	// float m_goalX, m_goalY, m_goalZ;
/*
  MatrixXd m(2,2);
  m(0,0) = 3;
  m(1,0) = 2.5;
  m(0,1) = -1;
  m(1,1) = m(1,0) + m(0,1);
  std::cout << m << std::endl;
*/
};


