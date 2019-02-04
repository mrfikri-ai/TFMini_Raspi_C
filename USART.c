#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int main()
{
	int fd;
	int count;
	unsigned int NextTime;

	if (wiringPiSetup() == -1)
	{
		printf("test");
		return 1;
	}
	printf("setup done \n");

	if ((fd = serialOpen("/dev/ttyS0", 115200)) < 0 )
	{
		fprintf(stderr, "Unable to open serial device: %s \n", strerror (errno));
		return 1;
	}
	printf("Port Open \n");

	while(1)
	{
		while(serialDataAvail(fd))
		{
			if(serialGetchar(fd) == 53)
			{
				printf("data masuk");
				serialFlush(fd);
			}
		}

		serialPutchar(fd,'5');
		printf("send \n");
		delay(500);
	}

}
