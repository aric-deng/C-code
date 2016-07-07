#include <stdio.h>
#include <fcntl.h>

unsigned int swap32(unsigned int num)
{
	unsigned int swapped;

	swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
		        ((num<<8)&0xff0000) | // move byte 1 to byte 2
				((num>>8)&0xff00) | // move byte 2 to byte 1
				((num<<24)&0xff000000); // byte 0 to byte 3
	
	return swapped;
}

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: exec filename !\n");
		return -1;
	}

	int cnt = 0;
	unsigned tmp = 0;
	unsigned int *buf = &tmp;
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}

	int out_fd = open("output.bin", O_RDWR | O_CREAT, 777);
	if (out_fd < 0){
		printf("open out_fd fail !\n");
		return -1;
	}

	while(1){
		cnt = read(fd, buf, sizeof(unsigned int));
		if (cnt == 0){
			printf("reach file end, return ! \n");
			break;
		}
		
		*buf = swap32(*buf);
		
		if (write(out_fd, buf, cnt) <= 0){
			printf("write out_fd fail! \n");
			break;
		}	
	}

	close(fd);
	close(out_fd);
	
	return 0;
}
