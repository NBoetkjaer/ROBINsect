#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "pca9685.hpp"

enum REG
{
    MODE1           = 0x00,
    MODE2           = 0x01,
    SUBADR1         = 0x02,
    SUBADR2         = 0x03,
    SUBADR3         = 0x04,
    ALLCALLADR      = 0x05,
    LED0_ON_L       = 0x06,
    LED0_ON_H       = 0x07,
    LED0_OFF_L      = 0x08,
    LED0_OFF_H      = 0x09,
    ALL_LED_ON_L    = 0xFA,
    ALL_LED_ON_H    = 0xFB,
    ALL_LED_OFF_L   = 0xFC,
    ALL_LED_OFF_H   = 0xFD,
    PRE_SCALE       = 0xFE,
    TestMode        = 0xFF
};

// Control register MODE1 bits
#define RESTART     (7)
#define EXTCLK      (6)
#define AI          (5)
#define SLEEP       (4)
#define SUB1        (3)
#define SUB2        (2)
#define SUB3        (1)
#define ALLCALL     (0)

// Control register MODE2 bits
#define INVRT       (4)
#define OCH         (3)
#define OUTDRV      (2)
#define OUTNE_1     (1)
#define OUTNE_0     (0)

#define INT_OSC_FREQ (25000000.0f) // Hz
#define CHANNEL_COUNT (16)
#define BV(bit) (1<<(bit))  // Bit value.
pca9685::pca9685()
    : m_file(-1)
{
    for (int i = 0; i < CHANNEL_COUNT; ++i)
    {	// Turn off all PWM channels.
        setPWM(i, 0, 0);
    }
}

pca9685::~pca9685()
{
}
int pca9685::init()
{
    int adapter_nr = 1; /* probably dynamically determined */
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    m_file = open(filename, O_RDWR);
    if (m_file < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return m_file;
    }

    int addr = 0x70; /* The I2C address */
    if (ioctl(m_file, I2C_SLAVE, addr) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return m_file;
    }


    __s32 mode1 = i2c_smbus_read_byte_data(m_file, MODE1);
    if (mode1 < 0) { return mode1; }
    // Set auto increment bit, clear sleep bit .
    mode1 |= BV(AI);
    mode1 &= ~BV(SLEEP);
    //mode1 |= BV(RESTART);	
    __s32 res = i2c_smbus_write_byte_data(m_file, MODE1, mode1);
    if (res < 0) { return res; }
    return 0;

    __s32 mode2 = i2c_smbus_read_byte_data(m_file, MODE2);
    if (mode2 < 0) { return mode2; }
    mode2 |= BV(OUTDRV);
    res = i2c_smbus_write_byte_data(m_file, MODE2, mode2);
    if (res < 0) { return res; }
    return 0;
}

float pca9685::setUpdateFrequency(float freq_Hz)
{
    int prescalValue = (int)((INT_OSC_FREQ / 4096.0f / freq_Hz) - 1.0f + 0.5f);
    if (prescalValue < 3 && prescalValue >  255) return -1.0f;
    // Read mode1 register 
    __s32 mode1 = i2c_smbus_read_byte_data(m_file, MODE1);
    if (mode1 < 0) {
        printf("Failed to read from MODE1 register.\n");
        return mode1;
    }
    // Check if sleep mode is enabled ).
    bool sleeping = ((mode1 & BV(SLEEP)) != 0);
    __s32 res;
    if (!sleeping) // Enter sleep mode (requirred during prescale update)
    {
        res = i2c_smbus_write_byte_data(m_file, MODE1, (mode1 & ~BV(RESTART)) | BV(SLEEP));
        if (res < 0) {
            printf("Failed to write to MODE1 register.\n");
            return res;
        }
    }
    // write new prescale value.
    res = i2c_smbus_write_byte_data(m_file, PRE_SCALE, (__u8)prescalValue);
    if (!sleeping) // Restart the module if it was previously put to sleep.
    {
        res = i2c_smbus_write_byte_data(m_file, MODE1, (mode1 | BV(RESTART)));
    }
    // Return actual frequency
    return (INT_OSC_FREQ / 4096.0f) / (prescalValue + 1);
}

void pca9685::setPWM(__u8 channel, unsigned short on, unsigned short off)
{
    if (on == off)
    {
        on = 0;
        off = 0x1000; // Set full off bit
    }
    if (off >= 0x0FFF && on == 0)
    {
        on = 0x1000; // Set full on bit
        off = 0x0;
    }

    m_channels[channel * 4 + 0] = on & 0xFF;
    m_channels[channel * 4 + 1] = on >> 8;
    m_channels[channel * 4 + 2] = off & 0xFF;
    m_channels[channel * 4 + 3] = off >> 8;
}

int pca9685::writeAllChannels()
{
    __s32 res;
    __u8 len = I2C_SMBUS_I2C_BLOCK_MAX;
    // 16 channels of 4 bytes each, we can read 32 bytes in one transaction, so we only need two transactions to transfer all channel data.
    for (__u8 i = 0; i < 2; ++i)
    {
        // Read a block of registers.
        res = i2c_smbus_write_i2c_block_data(m_file, LED0_ON_L + i*len, len, &m_channels[i*len]);
        if (res < 0) {
            printf("Failed to write block\n");
            return res;
        }
    }
    return 0;
}

int pca9685::readAllChannels()
{
    __s32 res;
    __u8 len = I2C_SMBUS_I2C_BLOCK_MAX;
    // 16 channels of 4 bytes each, we can read 32 bytes in one transaction, so we only need two transactions to transfer all channel data.
    for (__u8 i = 0; i < 2; ++i)
    {
        // Read a block of registers.
        res = i2c_smbus_read_i2c_block_data(m_file, LED0_ON_L + i*len, len, &m_channels[i*len]);
        if (res < 0) {
            printf("Failed to read block\n");
            return res;
        }

        //for(__u8 reg = 0; reg < len; ++reg)
        //{
        //	printf("Register 0x%2X = 0x%2X (%d)\n", LED0_ON_L + i*len + reg, m_channels[i*len+reg], m_channels[i*len+reg]);
        //}
    }
    return 0;
}

void pca9685::printAllRegisters()
{
    for (__u8 reg = 0; reg != 255; reg++)
    {
        __s32 res = i2c_smbus_read_byte_data(m_file, reg);
        if (res < 0) {
            /* ERROR HANDLING: i2c transaction failed */
        }
        else {
            printf("Register 0x%2X = 0x%2X (%d)\n", reg, res, res);
        }
    }
}
