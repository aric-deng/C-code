#ifndef __GPIO_CONTROL_H__
#define __GPIO_CONTROL_H__

#include <linux/ioctl.h>
#include "gpio.h"

#define GPIO_CONTROL_LOG printk


typedef struct _gpio_dir_control
{
	unsigned char chip;
    unsigned int  groupnumber;
    unsigned int  bitnumber;
	unsigned int dir;
} gpio_dir_control;


typedef struct _gpio_level_control
{
	unsigned char chip;
    unsigned int  groupnumber;
    unsigned int  bitnumber;
	unsigned int level;
} gpio_level_control;

#define GPIO_CONTROL_IOC_MAGIC            't'

#define GPIO_SET_DIR                _IOW(GPIO_CONTROL_IOC_MAGIC, 4, gpio_dir_control)
#define GPIO_SET_LEVEL              _IOW(GPIO_CONTROL_IOC_MAGIC, 5, gpio_level_control)
#define GPIO_READ_LEVEL             _IOW(GPIO_CONTROL_IOC_MAGIC, 6, gpio_level_control)
#define GPIO_CONTROL_UP              _IOW(GPIO_CONTROL_IOC_MAGIC, 7, gpio_level_control)
#define GPIO_CONTROL_DOWN             _IOW(GPIO_CONTROL_IOC_MAGIC, 8, gpio_level_control)
#define GPIO_CONTROL_LEFT              _IOW(GPIO_CONTROL_IOC_MAGIC, 9, gpio_level_control)
#define GPIO_CONTROL_RIGHT             _IOW(GPIO_CONTROL_IOC_MAGIC, 10, gpio_level_control)
#define GPIO_CONTROL_UPSTOP             _IOW(GPIO_CONTROL_IOC_MAGIC, 11, gpio_level_control)
#define GPIO_CONTROL_DOWNSTOP             _IOW(GPIO_CONTROL_IOC_MAGIC, 12, gpio_level_control)
#define GPIO_CONTROL_LEFTSTOP             _IOW(GPIO_CONTROL_IOC_MAGIC, 13, gpio_level_control)
#define GPIO_CONTROL_RIGHTSTOP             _IOW(GPIO_CONTROL_IOC_MAGIC, 14, gpio_level_control)
#define GPIO_CONTROL_SET_SARADC              _IOW(GPIO_CONTROL_IOC_MAGIC, 15, gpio_level_control)
#define GPIO_CONTROL_INIT_SARADC              _IOW(GPIO_CONTROL_IOC_MAGIC, 16, gpio_level_control)
#define GPIO_CONTROL_GET_SARADC              _IOW(GPIO_CONTROL_IOC_MAGIC, 17, gpio_level_control)
#define GPIO_CONTROL_CLEAR_SARADC              _IOW(GPIO_CONTROL_IOC_MAGIC, 18, gpio_level_control)
#define GPIO_CONTROL_IRCURT_DISABLE              _IOW(GPIO_CONTROL_IOC_MAGIC, 19, gpio_level_control)
#define GPIO_CONTROL_IRCURT_FILTEREN              _IOW(GPIO_CONTROL_IOC_MAGIC, 20, gpio_level_control)
#define GPIO_CONTROL_IRCURT_FILTERDIS              _IOW(GPIO_CONTROL_IOC_MAGIC, 21, gpio_level_control)
#define GPIO_CONTROL_LEFTUP                 _IOW(GPIO_CONTROL_IOC_MAGIC, 22, gpio_level_control)
#define GPIO_CONTROL_LEFTDOWN               _IOW(GPIO_CONTROL_IOC_MAGIC, 23, gpio_level_control)
#define GPIO_CONTROL_RIGHTUP                _IOW(GPIO_CONTROL_IOC_MAGIC, 24, gpio_level_control)
#define GPIO_CONTROL_RIGHTDOWN              _IOW(GPIO_CONTROL_IOC_MAGIC, 25, gpio_level_control)
#define GPIO_CONTROL_INITLEFT         _IOW(GPIO_CONTROL_IOC_MAGIC, 26, gpio_level_control)
#define GPIO_CONTROL_INITUP           _IOW(GPIO_CONTROL_IOC_MAGIC, 27, gpio_level_control)
#define GPIO_CONTROL_INITRIGHT        _IOW(GPIO_CONTROL_IOC_MAGIC, 28, gpio_level_control)
#define GPIO_CONTROL_INITDOWN         _IOW(GPIO_CONTROL_IOC_MAGIC, 29, gpio_level_control)

#endif

