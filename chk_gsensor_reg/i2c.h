#ifndef __I2C_H__
#define __I2C_H__
typedef enum I2C_BUS_ID
{
    I2C_SENSOR = 0,
    I2C_DEVICE,
} I2C_BUS_ID_E;

#define printf_w(string...)	\
do{\
	printf("%s(%d)[%s]: ", __FILE__, __LINE__, __FUNCTION__);\
	printf(string);\
} while(0)


int i2c_init(void);
int i2c_deInit(void);

int i2c_read_buf(I2C_BUS_ID_E id, unsigned char devaddress, unsigned short  reg_addr,  unsigned char *buf, int count);
int i2c_write_buf(I2C_BUS_ID_E id, unsigned char devaddress, unsigned short  reg_addr, unsigned char *buf, int count);

#endif
