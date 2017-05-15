#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: %s [filename] !\n", argv[0]);
		return -1;
	}

	int cnt = 0;
	unsigned short tmp = 0;
	char output_name[256] = {0};
	unsigned short *buf = &tmp;
	char * pSuffix;
	
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}
	
	strncpy(output_name, argv[1], sizeof(output_name));
	pSuffix = rindex(output_name, '.');
	sprintf(pSuffix, ".argb4444");
		
	int out_fd = open(output_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (out_fd < 0){
		printf("open %s fail !\n", output_name);
		return -1;
	}

	while(1){
		cnt = read(fd, buf, sizeof(unsigned short));
		if (cnt == 0){
			printf("reach file end, return ! \n");
			break;
		}
		
		*buf = 0xF000 | ((*buf) >> 4 & 0X0F00) | ((*buf) >> 3 & 0X00F0) | ((*buf) >> 1 & 0X000F);
		
		if (write(out_fd, buf, cnt) <= 0){
			printf("write out_fd fail! \n");
			break;
		}	
	}

	close(fd);
	close(out_fd);
	
	return 0;
}
