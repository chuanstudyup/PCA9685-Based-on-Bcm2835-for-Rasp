#ifndef PCA9685_H
#define PCA9685_H

#include <bcm2835.h>
#include <cstdio>
#include <cstdint>

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

/**
 * @brief  Class that stores state and functions for interacting with PCA9685 PWM chip
 */
class PCA9685{
	public:
		PCA9685(uint8_t addr = 0x40);
		~PCA9685();
		int init();
		void setPWM(uint8_t channel, uint32_t pulseWidth);
		
	private:
		uint8_t _addr;
		int _pcaFd;
		char sendBuf[3];
		uint8_t errCode;
};
#endif
