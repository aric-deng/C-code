/* GPIO_CONTROL.c
 *
 * Copyright (c) 2009 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 * 2010-02-12  Modify value of SHARPNESS Control Register[0x03] from 0x19 to 0x11, Hisilicon BVT MPP.
 * 2010-03-01  Modify value of Vertical Delay Register to 0x17, And add some interface, Hisilicon BVT MPP.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
//#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/list.h>
//#include <asm/semaphore.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
//#include <asm/hardware.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <linux/irq.h>
#include <mach/gpio.h>

#include "gpio_control.h"
#include "gpio.h"


static irqreturn_t gpio19_irq_handle(int irq, void *dummy)
{
    printk("gpio19_irq_handle\n");
    printk("irq : %d\n", gk7101_gpio_get(GPIO_19)); //(GPIO_24));

    mdelay(1);
}

static irqreturn_t gpio36_irq_handle(int irq, void *dummy)
{
    printk("gpio36_irq_handle\n");
    printk("irq : %d\n", gk7101_gpio_get(GPIO_36)); //(GPIO_24));

    mdelay(1);
}

static int gpio_ctrl_init(void)
{
    static struct gk7101_gpio_irq_info gpiox_irq;

    gpiox_irq.irq_gpio   = GPIO_19; //GPIO_24;
    gpiox_irq.irq_type   = IRQ_TYPE_EDGE_BOTH;
    gpiox_irq.handler    = gpio19_irq_handle;

    gk7101_gpio_request_irq(&gpiox_irq);
	
	gpiox_irq.irq_gpio   = GPIO_36; //GPIO_24;
    gpiox_irq.irq_type   = IRQ_TYPE_EDGE_BOTH;
    gpiox_irq.handler    = gpio36_irq_handle;

    gk7101_gpio_request_irq(&gpiox_irq);
}

static int __init gpio_control_module_init(void)
{
    int ret = 0;

	gpio_ctrl_init();
	
    printk("gpio control driver init successful!\n");

    return 0;
}


static void __exit gpio_control_module_exit(void)
{

}

module_init(gpio_control_module_init);
module_exit(gpio_control_module_exit);

#ifdef MODULE
//#include <linux/compile.h>
#endif
MODULE_LICENSE("GPL");

