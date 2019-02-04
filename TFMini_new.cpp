#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#define TF_MINI_SIZE 9
#define ITERATION 5 

enum TestData{
	STATE_READY,
	STATE_HEADER,
	STATE_DATA,
	STATE_CHECKSUM
};

double KalmanFilterInit(double distance)
{
    // Define Kalman Filter Parameter
    double m_kalman_filter; 
    double m_x_present = 0.0, m_x_previous = 0.0, m_x_future = 0.0;
    double m_noise = 6e-3, m_Q_param = 1e-4;
    double m_param_present = 0.0, m_param_previous =0.0, m_param_future = 0.0;
    double m_Kalman_gain = 0.0, m_Z_param = 0.0;
    
    // Start the recursive calculation of Kalman Filter
    
    // Define the predict parameters
    m_Z_param = distance; 
    m_x_present = m_x_previous;
    m_param_present = m_param_previous + m_Q_param;
    m_Kalman_gain = m_param_present/(m_param_present + m_noise);
    
    // Update the predict into estimation
    m_x_future = m_x_present + m_Kalman_gain * (m_Z_param - m_x_present);
    m_param_future = (1 - m_Kalman_gain) * m_param_present;
    m_kalman_filter = m_x_future;
    
    // Refresh the calculation, referring to previous parameter
    m_x_future = m_x_previous; m_param_future = m_param_previous;
    
    return m_kalman_filter;
}

double InformationEntropyInit(double probability)
{
  // Here we calculate the information entropy from kalman filter data
  // We would define the N of iteration 5 times
  // define the parameters
  
  double m_result[5] = {0}; // we keep the data 0 as the initialize
  int i = 0; // number of iteration
  double m_entropy;
  
  for (i = 0; i<ITERATION; i++)
  {
    m_result[i] = KalmanFilterInit(probability);
  }
  
  m_entropy = - ((m_result[0]*log2(m_result[0])) + (m_result[1]*log2(m_result[1]))
              + (m_result[2]*log2(m_result[2])) + (m_result[3]*log2(m_result[3]))
              + (m_result[4]*log2(m_result[4])))/ITERATION;
              
  return m_entropy;
}

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
	  double __kalman_res = KalmanFilterInit(distance);
    double __entropy_res = InformationEntropyInit(__kalman_res);
    
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
			printf("Distance: %d, Kalman: %f, Entropy: %f \n", distance, __kalman_res, __entropy_res);
			
      flag = false;
		}

	}
	return 0;
}
