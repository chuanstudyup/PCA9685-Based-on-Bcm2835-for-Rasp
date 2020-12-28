#include "pca9685.h"

int main()
{
	PCA9685 pca9685;
	bcm2835_init();
	if(!pca9685.init())
		printf("pca9685 init failed!\n");
	int i = 10;
	while(i--)
	{
		pca9685.setPWM(0,1000-i*20);
		printf("i = 0x%x\n",i);
	}
	return 0;
}
