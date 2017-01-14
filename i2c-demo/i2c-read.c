#include <sys/ioctl.h> 
#include <fcntl.h> 
#include <linux/i2c-dev.h> 
#include <linux/i2c.h> 

#define MAX_I2C_MSG          2

int i2c_read_reg_data(int bus_id, unsigned char slave_address,
		unsigned short reg_addr, int reg_addr_size, unsigned char *data)
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
	
	work_queue.nmsgs = MAX_I2C_MSG; //msg numbers

	work_queue.msgs = (struct i2c_msg*) malloc(
			work_queue.nmsgs * sizeof(struct i2c_msg));
	if (!work_queue.msgs)
	{
		printf("Memory alloc error\n");
		close(fd);
		return 0;
	}

	if (reg_addr_size == 1)
	{
		val[0] = 0x00ff & reg_addr;
	}
	else if (reg_addr_size == 2)
	{
		val[0] = 0x00ff & (reg_addr >> 8);
		val[1] = 0x00ff & reg_addr;
	}

	(work_queue.msgs[0]).len = reg_addr_size;
	(work_queue.msgs[0]).addr = slave_address;
	(work_queue.msgs[0]).buf = &val[0];

	(work_queue.msgs[1]).len = 1/*data_len*/;
	(work_queue.msgs[1]).flags = 1; //I2C_M_RD;
	(work_queue.msgs[1]).addr = slave_address;
	(work_queue.msgs[1]).buf = &val[0];
	//work_queue.nmsgs = 2;

	ioctl(fd, I2C_TIMEOUT, 2);
	ioctl(fd, I2C_RETRIES, 1);

	if (ioctl(fd, I2C_RDWR, (unsigned long) &work_queue) < 0)
	{
		ret=-1;
		printf("Error  during  I2C_RDWR  ioctl  with  error  code:  %d\n", ret);
	}
	else
	{
		//printf("Read: Reg--%02x   Data--%02x  \n", reg_addr, val[0]);
		if (data)
			*data = val[0];
	}
	free(work_queue.msgs);
	close(fd);
	return ret;
}

int main(int argc, char* argv[]) 
{     
	
	if (argc < 4){
		printf("Usage: i2c-read ${I2C_BUS_NUMBER} ${dev_addr} ${reg_addr} \n");
		return -1;
	}
	
	int i2c_bus = atoi(argv[1]);
	unsigned int dev_addr = strtoul(argv[2], NULL, 0); 
	unsigned int reg_addr = strtoul(argv[3], NULL, 0);
	//printf("dhs === 0x%02x, 0x%02x, 0x%02x \n", i2c_bus, dev_addr, reg_addr );

	unsigned char val = 0;
	int ret = i2c_read_reg_data(i2c_bus, dev_addr, reg_addr, 1, &val);
	printf("=== dhs === read i2c:%d, slave_addr:0x%02x, reg_addr:0x%02x, val:0x%02x\n", i2c_bus, dev_addr, reg_addr, val);
  
	return 0; 
}