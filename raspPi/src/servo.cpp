#include "servo.h"
#include <stdio.h>

Servo::Servo()
{
	SetRange(200, 650, -90.f, 90.f);
}

Servo::Servo(unsigned short pwmMin, unsigned short pwmMax, float angleMin, float angleMax)
{
	SetRange(pwmMin, pwmMax,  angleMin, angleMax);
}

void Servo::SetRange(unsigned short pwmMin, unsigned short pwmMax, float angleMin, float angleMax)
{
	m_pwmMin = pwmMin;
	m_pwmMax = pwmMax;
	m_angleMin = angleMin;
	m_angleMax = angleMax;

	slope = (m_pwmMax - m_pwmMin) / (m_angleMax - m_angleMin);

	if( m_angleMax>m_angleMin)
	{
		offset = m_pwmMin -  m_angleMin * slope ;
	}
	else
	{
		offset = m_pwmMax -  m_angleMax * slope;
	}
}

unsigned short Servo::GetPWM(float angle)
{
	float pwm = angle *slope + offset;
	if(pwm < m_pwmMin) pwm = m_pwmMin;
	if(pwm > m_pwmMax) pwm = m_pwmMax;
	return (unsigned short)(pwm +0.5f);
}
float Servo::GetAngle(unsigned short pwm)
{
	return (pwm - offset)/slope;
}
