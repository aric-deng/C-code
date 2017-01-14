#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/io.h>
#include <mach/hardware.h>

MODULE_LICENSE("GPL");
static 	dev_t hello_dev;
struct cdev  dev_c;

static void globalvar_read(void)
{
	printk("dhs %s \n", __func__);	
}

static void globalvar_write(void)
{
	printk("dhs %s \n", __func__);	
}

struct file_operations globalvar_fops =  
{ 
    read: globalvar_read, 
    write: globalvar_write, 
};

static int __init hello_init(void)
{

	int ret;
	printk("dhs %s \n", __func__);
	ret = alloc_chrdev_region(&hello_dev, 1, 16, "hello");
	cdev_init(&dev_c, &globalvar_fops);
	cdev_add(&dev_c, hello_dev, 1);
	return 0;
}

static void __exit hello_exit(void)
{
	printk("dhs %s \n", __func__);
	
}

module_init(hello_init);
module_exit(hello_exit);