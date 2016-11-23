#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
//#include <ncurses.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include <iomanip>

#include "pca9685.h"
#include "servo.h"
#include "LegKinematic.h"
#include "cmd.h"

using namespace std;

const unsigned short MAX_PWM = 650;
const unsigned short MIN_PWM = 200;

void performanceTest(pca9685 &servoCtrl);
void ArmTest(pca9685 &servoCtrl);
void ArmTest2(pca9685 &servoCtrl);
void mr996R_test(pca9685 &servoCtrl);
void menu_test(pca9685 &servoCtrl);

int main(int argc, char **argv)
{
	std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
	std::cout.precision(3);
	
	std::cout << ERASE_ALL HOME;
	
	pca9685 servoCtrl;

	if(servoCtrl.init() != 0)
	{
		printf("Failed  to open i2c device");
		 exit(1);
	}
	float freq;
	freq = servoCtrl.setUpdateFrequency(60.0f);
	printf("PWM Frequency set to %.2f \n", freq);
	servoCtrl.printAllRegisters();
	//mr996R_test(servoCtrl);
	//ArmTest(servoCtrl);
	//ArmTest2(servoCtrl);
	menu_test(servoCtrl);
	
	// Turn off all servos.
	for(__u8 u8Servo = 0; u8Servo < 16; ++u8Servo)
	{
		servoCtrl.setPWM(u8Servo, 0, 0);
	}
	servoCtrl.writeAllChannels();

	//performanceTest(servoCtrl);
	exit(0);
}

void mr996R_test(pca9685 &servoCtrl)
{
	__u8 u8Servo = 4;
	Servo servo(150, 650, -90.f,  90.f);
	servoCtrl.setPWM(u8Servo, 0, servo.GetPWM(-90));
	servoCtrl.writeAllChannels();
	usleep(3000000);
	float step = 1.f;
	int wait_us = 10000;
	printf("Performing angle sweep on all axes. Speed = %.2f deg/sec. \n", 1e6*step/wait_us);
	for(float angle  = -90; angle <= 90; angle += step)
	{
		unsigned short pwm =  servo.GetPWM(angle);
		servoCtrl.setPWM(u8Servo, 0, pwm);
		servoCtrl.writeAllChannels();
		printf("PWM = %d \n", pwm);
		usleep(wait_us);
	}
}

void ArmTest(pca9685 &servoCtrl)
{
	Servo servo[3];
	servo[0].SetRange(200, 750, 90.f,  -90.f); // base
	servo[1].SetRange(200, 690, -90.f,  90.f); // mid
	servo[2].SetRange(200, 620, -90.f,  90.f); // tip

	for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
	{
		servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(-90.f));
	}
	servoCtrl.writeAllChannels();
	usleep(3000000);
	int wait_us = 20000;
	float step = 1.f;
	printf("Performing angle sweep on all axes. Speed = %.2f deg/sec. \n", 1e6*step/wait_us);
	for(float angle  = -90; angle <= 90; angle += step)
	{
		for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
		{
			servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angle));
		}
		servoCtrl.writeAllChannels();
		usleep(wait_us);
	}
	usleep(1000000);
	for(float angle  = 90; angle >= -90; angle -= step)
	{
		for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
		{
			servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angle));
		}
		servoCtrl.writeAllChannels();
		usleep(wait_us);
	}

	usleep(1000000);
	for(float angle  = -90; angle <= 0; angle += step)
	{
		for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
		{
			servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angle));
		}
		servoCtrl.writeAllChannels();
		usleep(wait_us);
	}
	usleep(1000000);
}

void ArmTest2(pca9685 &servoCtrl)
{
	int wait_us = 30000;
	
	LegKinematic leg1;
	float x, y, z;
	leg1.setJoints(45, -45, -90);
	leg1.getGoal( x, y, z);

	Servo servo[3];
	servo[0].SetRange(200, 750, 90.f,  -90.f); // base
	servo[1].SetRange(200, 690, -90.f,  90.f); // mid
	servo[2].SetRange(200, 620, -90.f,  90.f); // tip

	for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
	{
		float angle_deg = leg1.getJoint(u8Servo);
		servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angle_deg));
	}
	servoCtrl.writeAllChannels();	
	usleep(2000000);

	std::cout << "Goal: (" << x <<", " << y <<", " << z << ")" << std::endl;
	int steps = 100;
	float step = 2.0f*y/steps;
	while(steps-- > 0)
	{
		y -= step;
		leg1.setGoal( x, y, z);
		float a1, a2, a3;
		leg1.getJoints(a1, a2, a3);
		std::cout << "Goal: (" << x <<", " << y <<", " << z << ")  ";
		std::cout << "Joint angles: (" << a1 <<", " << a2 <<", " << a3 << ")" << std::endl;
		
		for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
		{
			float angle_deg = leg1.getJoint(u8Servo);
			servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angle_deg));
		}
		servoCtrl.writeAllChannels();	
		usleep(wait_us);
		
	}	
}

void performanceTest(pca9685 &servoCtrl)
{
	struct timeval t0, t1;
	gettimeofday(&t0, NULL);
	int count = 500;
	int i = 0;
	for(; i < count; ++i)
	{
		if(servoCtrl.writeAllChannels())
		{
			printf("Test failed: %d\n", i);
			break;
		}
	}
	gettimeofday(&t1, NULL);
	long elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
	printf("Elapsed usec: %ld\n", elapsed);
	printf("Transfered bytes %d (%.2f B/sec)\n",16*4*i, 1000000.0f*16*4*i/ (float)elapsed);
}

void menu_test(pca9685 &servoCtrl)
{
	float angles[3] = {0.0f, 90.0f, -90.0f};
	Servo servo[3];
	LegKinematic leg1;
	float x, y, z;
	
	servo[0].SetRange(200, 750, 104.2,  -92.2f); // base
	servo[1].SetRange(200, 690, 101.4f,  -85.0f); // mid
	servo[2].SetRange(200, 620, -129.2f,  32.1f); // tip

	for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
	{
		servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angles[u8Servo]));
	}
	servoCtrl.writeAllChannels();
	leg1.setJoints(angles[0], angles[1], angles[2]);
	while(true)
	{
		//float x1, y1, z1;
		leg1.getGoal( x, y, z);
		// Draw menu.
		std::cout << ERASE_ALL HOME;
		std::cout << FOREGROUND(BLUE) "Press ENTER to quit." NEWLINE;
		std::cout << FOREGROUND(GREEN) "Joint 0: " <<  angles[0] << NEWLINE;
		std::cout << FOREGROUND(GREEN) "Joint 1: " <<  angles[1] << NEWLINE;
		std::cout << FOREGROUND(GREEN) "Joint 2: " <<  angles[2] << NEWLINE <<  NEWLINE;
		
		std::cout << FOREGROUND(GREEN) "Position (: " << x <<", " << y << ", " << z << ")"  << NEWLINE;		
			
		if(!_kbhit())
		{
			usleep(5000);
			continue;
		}
		
		int ch = getch();
		int joint = -1;
		float step = 2.0f;
		bool setPos = false;
		switch(ch)
		{
			case 10: return;
			// Joint 0
			case 'q':
				step = -step;
			case 'a':
				joint = 0;
				break;
			// Joint 1	
			case 'w':
				step = -step;
			case 's':
				joint = 1;
				break;	
			// Joint 2	
			case 'e':
				step = -step;
			case 'd':
				joint = 2;
				break;
			// X - axis.
			case 'x':
				x -= step; setPos = true;
				break;
			case 'X':
				x += step; setPos = true;				
				break;
			// Y - axis.	
			case 'y':
				y -= step; setPos = true;
				break;
			case 'Y':
				y += step; setPos = true;				
				break;
			// Z - axis.
			case 'z':
				z -= step; setPos = true;
				break;
			case 'Z':
				z += step; setPos = true;				
				break;				
		}
		
		if(joint != -1 || setPos)
		{
			if(setPos)
			{
				leg1.setGoal( x, y, z);
				leg1.getJoints(angles[0], angles[1], angles[2]);
			}
			else
			{
				angles[joint] += step;
				unsigned short pwm = servo[joint].GetPWM(angles[joint]);
				angles[joint] = servo[joint].GetAngle(pwm);
				leg1.setJoints(angles[0], angles[1], angles[2]);
			}
			for(__u8 u8Servo = 0; u8Servo < 3; ++u8Servo)
			{
				servoCtrl.setPWM(u8Servo, 0, servo[u8Servo].GetPWM(angles[u8Servo]));
			}
			servoCtrl.writeAllChannels();
			usleep(1000);
		}
		
		//
		
			
	}
}