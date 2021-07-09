#include "pca9685.h"

/**
 * @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on the Wire interface. On Due we use Wire1 since its the interface on the 'default' I2C pins.
 * @param  addr The 7-bit I2C address to locate this chip, default is 0x40
 **/
PCA9685::PCA9685(uint8_t addr)
{
	_addr = addr;
}

PCA9685::~PCA9685()
{
	bcm2835_i2c_end();
}

/**
 * Before MS5837::init(), please bcm2835_init() in main.c!
 **/
int PCA9685::init()
{
	printf("PCA9685 Init...\n");
	if(!bcm2835_i2c_begin())
	{
		printf("bcm2835_i2c_begin failed at %s%d\n",__FILE__,__LINE__);
		return 0;
	}
	bcm2835_i2c_set_baudrate(100000);
	
	setPWMFreq();
	return 1;
}

/**
 * @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
 * @param  freq Floating point frequency that we will attempt to match
 **/
void PCA9685::setPWMFreq(float freq)
{
	//freq *= 0.9;  //Correct for overshoot in the frequency setting (see issue #11).
					//说要校正，但是我用示波器测试PWM输出频率，好像不校正更准，可能和芯片的质量和精度有关，有条件的可以自行修正。
	_T =static_cast<uint32_t>(1/freq*1000000);
	double osc_clock = 25000000;
	float prescaleval = static_cast<unsigned char>(osc_clock/4096/freq-1);
	uint8_t prescale = static_cast<uint8_t>(floor(prescaleval+0.5));
	uint8_t oldmode = read8(PCA9685_MODE1);
	uint8_t newmode = (oldmode&0x7f) | 0x10; //准备进入sleep，设置时钟前必须先进入sleep模式
	write8(PCA9685_MODE1, newmode); // go to sleep
	write8(PCA9685_PRESCALE, prescale); // set the prescaler
	oldmode &= 0xef;
	write8(PCA9685_MODE1, oldmode);
	delay(2);
	write8(PCA9685_MODE1, (oldmode|0xa1));  //  This sets the MODE1 register to turn on auto increment.
}

/**
 * @brief  Set the pulseWidth of the channel
 * @param  channel  The pwm channel of PCA9685, 0-15
 * @param  pulseWidth The width of pluse(uints us), The width should be less than the period.
 **/
void PCA9685::setPWM(uint8_t channel, uint32_t pulseWidth)
{
	uint16_t off = static_cast<uint16_t> ((pulseWidth* 4096.0/_T )*1.01); //最后的1.01是校准用的
	setPWM(channel,0,off);
}

/**
 * @brief  Sets the PWM output of one of the PCA9685 pins
 * @param  channel One of the PWM output pins, from 0 to 15
 * @param  on At what point in the 4096-part cycle to turn the PWM output ON
 * @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 **/
void PCA9685::setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
	bcm2835_i2c_setSlaveAddress(_addr);
	sendBuf[0] = LED0_ON_L+4*channel;
	sendBuf[1] = on & 0x00FF;
	sendBuf[2] = on >> 8;
	sendBuf[3] = off & 0x00FF;
	sendBuf[4] = off >> 8;
	if((errCode = bcm2835_i2c_write(sendBuf,5)))
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__, errCode);
}

uint8_t PCA9685::read8(uint8_t addr)
{
	bcm2835_i2c_setSlaveAddress(_addr);
	sendBuf[0] = addr;
	char readByte;
	if((errCode = bcm2835_i2c_read_register_rs(sendBuf,&readByte,1)))
	{
		printf("bcm2835_i2c_read_register_rs failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__,errCode);
		return 0;
	}else
		return static_cast<uint8_t>(readByte);
}

void PCA9685::write8(uint8_t addr, uint8_t d)
{
	bcm2835_i2c_setSlaveAddress(_addr);
	sendBuf[0] = addr;
	sendBuf[1] = d;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
		printf("bcm2835_i2c_write failed, errCode = 0x%x\n", errCode);
}
