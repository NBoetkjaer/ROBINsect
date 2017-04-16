#pragma once

// PCA9685 16-Channel PWM Servo Driver
#include "../pca9685.hpp"

// This is a stub implementation.
pca9685::pca9685() {};
pca9685::~pca9685() {};
int pca9685::init(int adapter_nr, unsigned int i2c_addr) { return 0; };
int pca9685::selectAddress(unsigned int i2c_addr) { return 0; };
float pca9685::setUpdateFrequency(float freq_Hz) { return freq_Hz; };
void pca9685::setPWM(__u8 channel, unsigned short on, unsigned short off) {};
int pca9685::readAllChannels() { return 0; };
int pca9685::writeAllChannels() { return 0; };
void pca9685::printAllRegisters() { };
