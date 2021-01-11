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
	bcm2835_close();
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
	bcm2835_i2c_setSlaveAddress(_addr);
	bcm2835_i2c_set_baudrate(100000);
	
	double T = 20000;
	unsigned char prescale;
	double osc_clock = 25000000;
	char oldmode, newmode;
	//T /= 0.915;
	T /= 1000000;
	prescale = static_cast<unsigned char>(osc_clock/4096*T-1);
	sendBuf[0] = PCA9685_MODE1;
	if((errCode = bcm2835_i2c_read_register_rs(sendBuf,&oldmode,1)))
	{
		printf("bcm2835_i2c_read_register_rs failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__,errCode);
		return 0;
	}
	//oldmode = wiringPiI2CReadReg8(_pcaFd, PCA9685_MODE1);
	
	newmode = (oldmode&0x7f) | 0x10; //准备进入sleep，设置时钟前必须先进入sleep模式
	sendBuf[0] = PCA9685_MODE1;
	sendBuf[1] = newmode;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__,errCode);
		return 0;
	}

	sendBuf[0] = PCA9685_PRESCALE;
	sendBuf[1] = prescale;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed, errCode = 0x%x\n", errCode);
		return 0;
	}

	oldmode &= 0xef; //清除sleep位
	sendBuf[0] = PCA9685_MODE1;
	sendBuf[1] = oldmode;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__, errCode);
		return 0;
	}

	delay(1);
	sendBuf[1] = oldmode | 0xa1;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__, errCode);
		return 0;
	}
	
	printf("PCA9685 deivce init successfully\n");
	return 1;
}

/**
 * @brief  Set the pulseWidth of the channel
 * @param  channel  The pwm channel of PCA9685, 0-15
 * @param  pulseWidth The width of pluse, 0-20 000 us
 **/
void PCA9685::setPWM(uint8_t channel, uint32_t pulseWidth)
{
	uint16_t ON, OFF;
	ON = 0;
	OFF = static_cast<uint16_t> ((pulseWidth/20000.0 * 4096)*1.01); //最后的1.0067114是校准用的
	bcm2835_i2c_setSlaveAddress(_addr);
	
	sendBuf[0] = LED0_ON_L+4*channel;
	sendBuf[1] = ON & 0x00FF;
	sendBuf[2] = ON & 0x00FF;
	if((errCode = bcm2835_i2c_write(sendBuf,3)))
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__, errCode);	

	sendBuf[0] = LED0_OFF_L+4*channel;
	sendBuf[1] = OFF & 0x00FF;
	sendBuf[2] = OFF >> 8;
	if((errCode = bcm2835_i2c_write(sendBuf,3)))
		printf("bcm2835_i2c_write failed at %s%d, errCode = 0x%x\n",__FILE__,__LINE__, errCode);

}

