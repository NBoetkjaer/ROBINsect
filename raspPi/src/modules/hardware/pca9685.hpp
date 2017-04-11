#pragma once
// PCA9685 16-Channel PWM Servo Driver
#ifndef WIN32
#include <linux/i2c-dev.h>
#else
typedef unsigned char __u8;
#endif

class pca9685
{
public:
    pca9685();
    ~pca9685();
    int init(int adapter_nr = 1, int i2c_addr = 0x70);
    float setUpdateFrequency(float freq_Hz);
    void setPWM(__u8 channel, unsigned short on, unsigned short off);
    int readAllChannels();
    int writeAllChannels();
    void printAllRegisters();
private:
#ifndef WIN32
    __s32 write8u(__u8 reg, __u8 val){ return i2c_smbus_write_byte_data(m_file, reg, val); };
    __s32 read8u(__u8 reg){ return i2c_smbus_read_byte_data(m_file, reg); };
#endif
    int m_file;
    __u8 m_channels[16 * 4];
};
