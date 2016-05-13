#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define GK_ADC_IOC_MAGIC             'A'
#define IOC_ADC_GET_DATA		     _IOR(GK_ADC_IOC_MAGIC, 2, unsigned int)
#define LOW_VOLTAGE_ADC_VALUE       6
int get_adc_value(void)
{
    int fd;
	unsigned int adc_val;
	
	fd = open("/dev/adc", O_RDWR);
    if (fd < 0)
    {
        printf("open failed! \n");
        return -1;
    }

    ioctl(fd, IOC_ADC_GET_DATA, &adc_val);

    printf("----------------adc = %ld\n", adc_val);

    if(adc_val >= LOW_VOLTAGE_ADC_VALUE)
    {
        close(fd);
       // return 1;
    }
    else
    {
       // printf("----------------adc = %d\n",adc_val);
        close(fd);
       // return 0;
    }

	return adc_val;
}

int main(void)
{
	//int val = 0;

	while (1){
		get_adc_value();
		//printf("dhs get_adc_value: %d \n", get_adc_value());
		sleep(2);
	}
}
