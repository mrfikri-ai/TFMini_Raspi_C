#include <stdio.h>
#include <wiringPi.h>

int main()
{
	printf("raspberry pi blink tutorial \n");
	
	if (wiringPiSetup () == -1)
	{
		return 1;
	}
	
	printf("testing testing");
}
