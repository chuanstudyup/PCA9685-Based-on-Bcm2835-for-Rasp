#include "pca9685.h"

int main()
{
	PCA9685 pca9685;
	bcm2835_init();
	if(!pca9685.init())
		printf("pca9685 init failed!\n");
	pca9685.setPWM(0,1500);
	return 0;
}
// g++ -Wall -o "%e" "%f" pca9685.cpp  -l bcm2835
