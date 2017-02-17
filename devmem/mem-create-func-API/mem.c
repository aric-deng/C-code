#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

unsigned long reg_read(unsigned long addr)
{
	int fd;
	void *map_base, *virt_addr; 
	unsigned long read_result;
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		printf("/dev/mem open fail.\n"); 
		return -1;
	}
	
	//map 1 page
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK);
    if(map_base == (void *) -1) {
		printf("mmap map_base == -1.\n"); 
		return -1;
	}
	
	virt_addr = map_base + (addr & MAP_MASK);
	read_result = *((unsigned long *) virt_addr);
	printf("Value at address 0x%X (%p): 0x%X\n", addr, virt_addr, read_result); 
	
	if(munmap(map_base, MAP_SIZE) == -1) {
		printf("munmap fail.\n"); 
		return -1;
	}
	
    close(fd);
	
	return read_result;
}


unsigned long reg_write(unsigned long addr, unsigned long value)
{
	int fd;
	void *map_base, *virt_addr; 
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		printf("/dev/mem open fail.\n"); 
		return -1;
	}
	
	//map 1 page
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK);
    if(map_base == (void *) -1) {
		printf("mmap map_base == -1.\n"); 
		return -1;
	}
	
	virt_addr = map_base + (addr & MAP_MASK);
	*((unsigned long *) virt_addr) = value;

	printf("Written 0x%X; readback 0x%X\n", value, *((unsigned long *) virt_addr));
	
	if(munmap(map_base, MAP_SIZE) == -1) {
		printf("munmap fail.\n"); 
		return -1;
	}
	
    close(fd);
	
    return 0;
}

int main(int argc, char *argv[])
{
	unsigned long addr, value;
	if(argc < 2) {//若参数个数少于两个则打印此工具的使用方法
        fprintf(stderr, "\nUsage:\t%s { address } [ data ] \n"
            "\taddress : memory address to act upon\n"
            "\tdata    : data to be written\n\n",
            argv[0]);
        exit(1);
    }
	
	addr = strtoul(argv[1], 0, 0);

	if (argc == 2){
		reg_read(addr);
	} else {
		value = strtoul(argv[2], 0, 0);
		reg_write(addr, value);
	}
}