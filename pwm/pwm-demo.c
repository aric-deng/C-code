#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#define PWM_CH	0

//pwm
#define PWM_IOCTL_MAGIC      'k'
#define PWM_IOCTL_ENABLE     _IO(PWM_IOCTL_MAGIC, 0x10)
#define PWM_IOCTL_DISABLE    _IO(PWM_IOCTL_MAGIC, 0x11)
#define PWM_IOCTL_SET_DUTY   _IO(PWM_IOCTL_MAGIC, 0x12)
#define PWM_IOCTL_GET_DUTY   _IO(PWM_IOCTL_MAGIC, 0x13)
#define PWM_IOCTL_SET_MODE   _IO(PWM_IOCTL_MAGIC, 0x14)
#define PWM_IOCTL_GET_MODE   _IO(PWM_IOCTL_MAGIC, 0x15)
#define PWM_IOCTL_SET_SPEED  _IO(PWM_IOCTL_MAGIC, 0x16)
#define PWM_IOCTL_GET_SPEED  _IO(PWM_IOCTL_MAGIC, 0x17)
#define PWM_IOCTL_GET_STATUS _IO(PWM_IOCTL_MAGIC, 0x18)

#define u8 unsigned char
#define u32 unsigned int
#define u16 unsigned short

typedef struct pwm_mode_s
{
    u8   channel;    /* 0 ~ 3*/
    u8   mode;       /* 0: normal speed  1: sync speed */
}pwm_mode_t;

typedef struct pwm_speed_s
{
    u8 channel; /* 0 ~ 3 */
    u32 speed;  /* 0x01 ~ 0x3ff*/
}pwm_speed_t;

typedef struct
{
    u8   channel;    /* 0 ~ 3 */
    u16  xon;        /* 0x00 ~ 0xffff */
    u16  xoff;       /* 0x00 ~ 0xffff */
}pwm_duty_t;


int main(void)
{
	int g_gpio_pwm_fd  = -1;
	g_gpio_pwm_fd = open("/dev/gk_pwm", 6);
	if ( g_gpio_pwm_fd < 0 )
	{
		printf("open g_gpio_pwm_fd failed! Reason:%s \n", strerror(errno));
		return -1;
	}

	printf("set pwm_mod \n");
	pwm_mode_t pwm_mod= {PWM_CH, 0}; //channel, mode
	ioctl(g_gpio_pwm_fd, PWM_IOCTL_SET_MODE, &pwm_mod);
	
	printf("set pwm_speed \n");
	//pwm_speed_t pwm_speed= {PWM_CH, 0x9470}; 
	pwm_speed_t pwm_speed= {PWM_CH, 38 * 1000}; 
	ioctl(g_gpio_pwm_fd,PWM_IOCTL_SET_SPEED, &pwm_speed);
#if	1
	printf("set pwm_duty \n");
	pwm_duty_t pwm_duty= {PWM_CH, 300, 700};  
	//pwm_duty_t pwm_duty= {PWM_CH,3,15};  
	ioctl(g_gpio_pwm_fd, PWM_IOCTL_SET_DUTY, &pwm_duty);
#endif	
	printf("set PWM_IOCTL_ENABLE \n");
	ioctl(g_gpio_pwm_fd, PWM_IOCTL_ENABLE, PWM_CH);	
	
	printf("enter while 1 \n");
	while(1);
	
	close(g_gpio_pwm_fd);
	return 0;
}
