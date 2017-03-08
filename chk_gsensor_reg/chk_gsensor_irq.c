#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2c.h"

#define I2C_DEV0        "/dev/i2c-0"
#define I2C_DEV1        "/dev/i2c-1"

#define I2C_SLAVE_FORCE 0x0706  /* Change slave address         */

#define GSENSOR_PARKING_MONITOR_FLAG	"/tmp/parking_monitor_flag"

pthread_mutex_t  g_i2c_0_mutex;
pthread_mutex_t  g_i2c_1_mutex;
static int g_i2c_init_flag = 0;//init i2c报错标志
static int g_i2c_error_flag = 0;//读取i2c报错标志
static int g_i2c_0_fd = -1;
static int g_i2c_1_fd = -1;


int i2c_init(void)
{
    int ret;
    i2c_deInit();

    g_i2c_0_fd = open("/dev/i2c-0", O_RDWR);
    if (g_i2c_0_fd == -1)
    {
        printf(" open /dev/i2c-0 failed!!!!\n");
        return -1;

    }
    g_i2c_1_fd = open("/dev/i2c-1", O_RDWR);
    if (g_i2c_1_fd == -1)
    {
        printf(" open /dev/i2c-1 failed!!!!\n");
        close(g_i2c_0_fd);
        return -1;
    }
    ret = pthread_mutex_init(&g_i2c_0_mutex, NULL);
    if (ret != 0)
    {
        printf("pthread_mutex_init g_i2c_0_mutex failed!!!\n");
        close(g_i2c_0_fd);
        close(g_i2c_1_fd);
        return -1;
    }
    ret = pthread_mutex_init(&g_i2c_1_mutex, NULL);
    if (ret != 0)
    {
        printf("pthread_mutex_init g_i2c_1_mutex failed!!!\n");
        pthread_mutex_destroy(&g_i2c_0_mutex);
        close(g_i2c_0_fd);
        close(g_i2c_1_fd);
        return -1;
    }

    g_i2c_init_flag = 1;
	//printf("i2c_init success\n");
    return g_i2c_init_flag;
}

int i2c_deInit(void)
{
    if (g_i2c_init_flag == 1)
    {
        pthread_mutex_destroy(&g_i2c_0_mutex);
        pthread_mutex_destroy(&g_i2c_1_mutex);
        close(g_i2c_0_fd);
        close(g_i2c_1_fd);
        g_i2c_0_fd = -1;
        g_i2c_1_fd = -1;
        g_i2c_init_flag = 0;
    }
	
	return 0;
}

/***********************************************************/
/**
* @param   i2c_fd   文件描述符 i2c_open获取
* @param   devaddress   设备基地址
* @param   address   此设备寄存器地址
*
* @return rxdata--读一个字节所读到的值   0--读取失败
* @brief -从设备寄存器里读一个字节
* @modify history
*  --------------------------
*
************************************************************/
static unsigned char i2c_read(int i2c_fd, unsigned char devaddress, unsigned short  address)
{
    unsigned char rxdata;

	//printf("devaddress:0x%x\n", devaddress);
    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, devaddress) < 0)
    {
        printf( "Fail to set i2c_addr\n");
        g_i2c_error_flag = -1;
        //close(g_i2c_fd);
        return '\0';
    }

    char sendbuffer[2];
    sendbuffer[0] = address >> 8;
    sendbuffer[1] = address;
//   printf("sendbuffer[0] %x sendbuffer[1] %x\n", sendbuffer[0], sendbuffer[1]);
    if (sendbuffer[0] == 0)//address is one byte
    {
        if (write(i2c_fd, &address, 1) != 1)
        {
            printf( "Fail to read send address data\n");
            printf("2Fail to read send address data\n");
            g_i2c_error_flag = -1;
            return '\0';
        }
    }
    else//address is one byte
    {
        if (write(i2c_fd, &sendbuffer, 2) != 2)
        {
            printf( "Fail to read send address data\n");
            printf("2Fail to read send address data\n");
            g_i2c_error_flag = -1;
            return '\0';
        }

    }

    if (read(i2c_fd, &rxdata, 1) != 1)
    {
        printf( "Fail to read read data\n");
        g_i2c_error_flag = -1;
        return '\0';
    }
    g_i2c_error_flag = 0;
    return rxdata;
}


/***********************************************************/
/**
* @param   i2c_fd   文件描述符i2c_open获取
* @param   devaddress   设备基地址
* @param   address   此设备寄存器地址
* @param   data   要写的数据
*
* @return 0--成功  -1--失败
* @brief -写一个字节到指定设备的寄存器中
* @modify history
*
************************************************************/
static int i2c_write(int i2c_fd, unsigned char devaddress, unsigned short  address, unsigned char data)
{
	printf("devaddress:0x%x\n", devaddress);
    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, devaddress) < 0)
    {
        printf( "Fail to set i2c_addr\n");
        return -1;
    }

    char sendbuffer[2];
    sendbuffer[0] = address >> 8;
    sendbuffer[1] = address;
//    printf("sendbuffer[0] %x sendbuffer[1] %x\n", sendbuffer[0], sendbuffer[1]);
    if (sendbuffer[0] == 0)//address is one byte
    {
        unsigned char ch[2] = {0};
        ch[0] = address;
        ch[1] = data;
        if (write(i2c_fd, &ch, 2) != 2)
        {
            printf( "Fail to read send address data\n");
            printf("2Fail to read send address data\n");
            g_i2c_error_flag = -1;
            return '\0';
        }
    }
    else//address is tow byte
    {
        unsigned char ch[3] = {0};
        ch[0] = sendbuffer[0] ;
        ch[1] = sendbuffer[1] ;
        ch[2] = data;
        if (write(i2c_fd, &ch, 3) != 3)
        {
            printf( "Fail to read send address data\n");
            printf("2Fail to read send address data\n");
            g_i2c_error_flag = -1;
            return '\0';
        }

    }

    return 0;
}


/***********************************************************/
/**
* @param   id   i2c设备类型
* @param   devaddress   要读的总线上设备基地址,7bit
* @param   reg_addr   寄存器的地址
* @param   buf   接收的字符数组
* @param   count   读多少个数据
*
* @return 0-成功    -1---失败
* @brief -从设备地址寄存器读出一定数量的字节出来
* @modify history
*
************************************************************/
int i2c_read_buf(I2C_BUS_ID_E id, unsigned char devaddress, unsigned short  reg_addr,  unsigned char *buf, int count)
{
     if (buf == NULL || count < 1)
       return -1;
    int i = 0;
    int i2c_fd = (id == I2C_SENSOR ? g_i2c_0_fd : g_i2c_1_fd);
    pthread_mutex_t  g_i2c_mutex = (id == I2C_SENSOR ? g_i2c_0_mutex : g_i2c_1_mutex);

    if (i2c_fd == -1)
    {
        printf("open i2c dev err\n ");
        return -1;
    }

//     printf("i2c_fd %d\n", i2c_fd);
    pthread_mutex_lock(&g_i2c_mutex);

    while (count--)
    {
        *buf++ = i2c_read(i2c_fd, devaddress, reg_addr++);
    }
    pthread_mutex_unlock(&g_i2c_mutex);

    for (i = 0; i < count; i++)
    {
        if (*buf == '\0')
            break;
    }

    if (i < count)
    {
        printf( "read num is not correct!\n");
        return -1;
    }

    if (g_i2c_error_flag == -1)
    {
        printf("## i2c operate error\n");
        return -1;
    }

    return 0;
}


/***********************************************************/
/**
* @param   id   i2c设备类型
* @param   devaddress   设备基地址 7bit
* @param   reg_addr   寄存器地址
* @param   buf   要写的字符数组
* @param   count   要写的字符个数
*
* @return 0--成功  -1----失败
* @brief -写一定数量的字节到指定设备的寄存器中
* @modify history
*
************************************************************/
int i2c_write_buf(I2C_BUS_ID_E id, unsigned char devaddress, unsigned short  reg_addr, unsigned char *buf, int count)
{

    if (count < 1)
        return -1;
    int ret = 0;
	
    int i2c_fd = (id == I2C_SENSOR ? g_i2c_0_fd : g_i2c_1_fd);
    pthread_mutex_t  g_i2c_mutex = (id == I2C_SENSOR ? g_i2c_0_mutex : g_i2c_1_mutex);

    if (i2c_fd == -1)
    {
        printf("open i2c dev err\n ");
        return -1;
    }
    pthread_mutex_lock(&g_i2c_mutex);	
    while (count--)
    {
        ret = i2c_write(i2c_fd, devaddress, reg_addr++, *buf++);
        if (ret != 0)
        {
            printf( "i2c_write num is not correct!\n");
            close(i2c_fd);
            pthread_mutex_unlock(&g_i2c_mutex);
            return -1;
        }
    }
    pthread_mutex_unlock(&g_i2c_mutex);
    return 0;
}

int main(int argc, char **argv)
{
	unsigned char bus_num, dev_i2c_addr, reg_addr, irq_bit_mask;
	int ret;
	int fd;
	unsigned char read_buf;
	
	if (argc < 5){
		printf("Usage: %s ${I2C_BUS_NUMBER} ${DEV_I2C_ADDR} ${REG_ADDR} ${INT_TRIGGER_BIT_MASK}\n", argv[0]);
		return -1;
	}
	
	bus_num = strtoul(argv[1], 0, 0);
	dev_i2c_addr = strtoul(argv[2], 0, 0);
	reg_addr = strtoul(argv[3], 0, 0);
	irq_bit_mask = strtoul(argv[4], 0, 0);
	
	printf("gsensor bus_num:%d, dev_i2c_addr:0x%02x, reg_addr:0x%02x, irq_bit_mask:0x%02x \n", bus_num, dev_i2c_addr, reg_addr, irq_bit_mask);
	
    i2c_init();
	
	ret = i2c_read_buf(bus_num, dev_i2c_addr, reg_addr, &read_buf, sizeof(read_buf));
	if(ret != 0){
		printf("i2c_read_buf fail, ret:%d\n", ret);
		i2c_deInit();
		return -1;
	}
	
	printf("read reg_addr:0x%02x = 0x%02x. \n", reg_addr, read_buf);
	if(read_buf & irq_bit_mask)
	{
		printf("\n\n ====== gsensor trigger!\n\n");
		
		if(access(GSENSOR_PARKING_MONITOR_FLAG, F_OK) != 0)
		{
			fd = open(GSENSOR_PARKING_MONITOR_FLAG, O_CREAT, 0766);
			if(fd < 0)
			{
				printf("create %s failed!\n", GSENSOR_PARKING_MONITOR_FLAG);
				return -1;
			}
			close(fd);
            sync();
		}
	}
	
	i2c_deInit();
	
	return 0;
}


