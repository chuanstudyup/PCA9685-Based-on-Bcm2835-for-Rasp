#ifndef PCA9685_H
#define PCA9685_H

#include <bcm2835.h>
#include <cstdio>
#include <cstdint>
#include <cmath>

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

/**
 * @brief  Class that stores state and functions for interacting with PCA9685 PWM chip
 */
class PCA9685{
	public:
		PCA9685(uint8_t addr = 0x40);
		~PCA9685();
		int init();
		void setPWMFreq(float freq = 50);
		void setPWM(uint8_t channel, uint32_t pulseWidth);
		void setPWM(uint8_t channel, uint16_t on, uint16_t off);
		
	private:
		uint8_t _addr;
		uint32_t _T;  //PWM Period[us]
		char sendBuf[5];
		uint8_t errCode;
		
		uint8_t read8(uint8_t addr);
		void write8(uint8_t addr, uint8_t d);
};
#endif
