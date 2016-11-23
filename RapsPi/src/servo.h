class Servo
{
	public:
		Servo();
		Servo(unsigned short pwmMin, unsigned short pwmMax, float angleMin, float angleMax);
		void SetRange(unsigned short pwmMin, unsigned short pwmMax, float angleMin, float angleMax);
		unsigned short GetPWM(float angle);
		float GetAngle(unsigned short pwm);
	private:
		unsigned short m_pwmMin, m_pwmMax;
		float m_angleMin, m_angleMax;
		float slope, offset;
};
