#include <stdio.h>
#include <sys/ioctl.h> 
#include <fcntl.h> 
#include <linux/i2c-dev.h> 
#include <linux/i2c.h> 

#define MAX_I2C_MSG          2
#define I2C_DBG		printf

static unsigned char i2c_read(int i2c_fd, unsigned char dev_addr, unsigned short  reg_addr)
{
    unsigned char rxdata;

    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, dev_addr) < 0)
    {
        I2C_DBG("Fail to set i2c_addr\n");
        //close(g_i2c_fd);
        return '\0';
    }

    char sendbuffer[2];
    sendbuffer[0] = reg_addr >> 8;
    sendbuffer[1] = reg_addr;
//   I2C_DBG("sendbuffer[0] %x sendbuffer[1] %x\n", sendbuffer[0], sendbuffer[1]);
    if (sendbuffer[0] == 0)//reg_addr is one byte
    {
        if (write(i2c_fd, &reg_addr, 1) != 1)
        {
            I2C_DBG("Fail to read send reg_addr data\n");
            return '\0';
        }
    }
    else//reg_addr is one byte
    {
        if (write(i2c_fd, &sendbuffer, 2) != 2)
        {
            I2C_DBG("Fail to read send reg_addr data\n");
            return '\0';
        }

    }

    if (read(i2c_fd, &rxdata, 1) != 1)
    {
        I2C_DBG("Fail to read read data\n");
        return '\0';
    }
	
    return rxdata;
}

int i2c_read_reg_data(int bus_id, unsigned char slave_address,
		unsigned short reg_addr, unsigned char *data)
{
	struct i2c_rdwr_ioctl_data work_queue;
	unsigned char idx;
	unsigned int fd;
	unsigned char val[10] =
	{ 0 };
	int ret = 0;

	if (bus_id == 0)
	{
		fd = open("/dev/i2c-0", O_RDWR);
	}
	else if (bus_id == 1)
	{
		fd = open("/dev/i2c-1", O_RDWR);
	}
	else
		return -1;
	
	if (fd < 0) {
		printf("open I2C device fail! \n");
		return -1;
	}
	
	*data = i2c_read(fd, slave_address, reg_addr);
	if (*data == '\0'){
		ret = -1;		
	}
	
	close(fd);
	return ret;
}

static int i2c_write(int i2c_fd, unsigned char devaddress, unsigned short  address, unsigned char data)
{
    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, devaddress) < 0)
    {
        I2C_DBG("Fail to set i2c_addr\n");
        return -1;
    }

    char sendbuffer[2];
    sendbuffer[0] = address >> 8;
    sendbuffer[1] = address;
//    I2C_DBG("sendbuffer[0] %x sendbuffer[1] %x\n", sendbuffer[0], sendbuffer[1]);
    if (sendbuffer[0] == 0)//address is one byte
    {
        unsigned char ch[2] = {0};
        ch[0] = address;
        ch[1] = data;
        if (write(i2c_fd, &ch, 2) != 2)
        {
            I2C_DBG("Fail to read send address data\n");
            return -1;
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
            I2C_DBG("Fail to read send address data\n");
            return -1;
        }

    }

    return 0;
}

int i2c_write_reg_data(int bus_id, unsigned char slave_address,
		unsigned short reg_addr, unsigned char reg_value)
{
	unsigned int fd;
    unsigned char ch[2] = {0};
	int ret = 0;

	if (bus_id == 0)
	{
		fd = open("/dev/i2c-0", O_RDWR);
	}
	else if (bus_id == 1)
	{
		fd = open("/dev/i2c-1", O_RDWR);
	}
	else
		return -1;
	
	if (fd < 0) {
		printf("open I2C device fail! \n");
		return -1;
	}

	ret = i2c_write(fd, slave_address, reg_addr, reg_value);
	
	close(fd);
	return ret;
}

int main(int argc, char* argv[]) 
{     
	
	if (argc < 5){
		printf("Usage: i2c-write ${I2C_BUS_NUMBER} ${dev_addr} ${reg_addr} ${value}\n");
		return -1;
	}
	
	int i2c_bus = atoi(argv[1]);
	unsigned int dev_addr = strtoul(argv[2], NULL, 0); 
	unsigned int reg_addr = strtoul(argv[3], NULL, 0);
	unsigned int reg_value = strtoul(argv[4], NULL, 0);
	//printf("dhs === 0x%02x, 0x%02x, 0x%02x \n", i2c_bus, dev_addr, reg_addr );

	unsigned char val = 0;
	//int ret = i2c_read_reg_data(i2c_bus, dev_addr, reg_addr, 1, &val);
	int ret = i2c_write_reg_data(i2c_bus, dev_addr, reg_addr, reg_value);
	ret = i2c_read_reg_data(i2c_bus, dev_addr, reg_addr, &val);
	printf("=== dhs === set i2c:%d, slave_addr:0x%02x, reg_addr:0x%02x, val:0x%02x\n", i2c_bus, dev_addr, reg_addr, val);
  
	return 0; 
}
