#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#define TF_MINI_SIZE 9

enum testData{
	STATE_READY,
	STATE_HEADER,
	STATE_DATA,
	STATE_CHECKSUM
};

int main()
{
	int fd;
	int state = STATE_READY;
	int cnt = 0; 
	char buffer[100] = {0};
	bool flag = false;

	if(wiringPiSetup () == -1)
	{
		printf("test");
		return 1;
	}
	printf("done\n");
	
	if ((fd = serialOpen("/dev/ttyS0", 115200)) < 0)
	{
		return 1;		
	}
	printf("Port Open\n");

	
	while(1)
	{
			
		while(serialDataAvail(fd))
		{
			buffer[cnt] = serialGetchar(fd);
			switch(state)
			{
				case STATE_READY:
				if(buffer[cnt] == 89)
				{
					state = STATE_HEADER;
					cnt = cnt+1;
				}
				break;

				case STATE_HEADER:
				if(buffer[cnt] == 89)
				{
					state = STATE_DATA;
					cnt = cnt+1;
				}

				else
				{
					state = STATE_READY;
					cnt = 0;
				}
				break;

				case STATE_DATA:
				if(cnt > TF_MINI_SIZE-3)
                                {
					state = STATE_CHECKSUM;
				}
				cnt = cnt+1;
				break;

				case STATE_CHECKSUM:
				{
					int i = 0;
					int check = 0;
					for(i = 0; i < TF_MINI_SIZE-1; i++)
					{
						check = check +  buffer[i];
					}
					check = check & 0x00ff;
					if(buffer[cnt] == check)
					{
						flag = true;
					}
					state = STATE_READY;
					cnt = 0;
					break;
				}
				break;
			}

		}

		if(flag == true)
		{
			int distance = (int)(buffer[2] | ((int)buffer[3] << 8));
			int strength = (int)(buffer[4] | ((int)buffer[5] << 8));
			printf("Distance: %d, Strength: %d\n", distance, strength);
			flag = false;
		}

	}
	return 0;
}
