#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

/*
    * Event types
	 */

#define EV_SYN			0x00
#define EV_KEY			0x01
#define EV_REL			0x02
#define EV_ABS			0x03
#define EV_MSC			0x04
#define EV_SW			0x05
#define EV_LED			0x11
#define EV_SND			0x12
#define EV_REP			0x14
#define EV_FF			0x15
#define EV_PWR			0x16
#define EV_FF_STATUS		0x17
#define EV_MAX			0x1f
#define EV_CNT			(EV_MAX+1)

int main(int argc, char *argv[])
{
	int fd;
	int len = 0;
	struct input_event event_val;

	fd = open("/dev/input/event0",0);
	if(fd == -1)
		return -1;
	while (1){
		len = read(fd, &event_val, sizeof(struct input_event));
		if (len > 0) {
		    //if (event_val.type == EV_KEY && event_val.value != 0)
			if (event_val.type == EV_KEY) 
			{
				printf("event key, code:0x%02x  \n", event_val.code);
			}
		}
	}
	return 0;
}

