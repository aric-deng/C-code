#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define OUTPUT_SUFFIX "-swap"

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
		printf("Usage: %s [filename] !\n", argv[0]);
		return -1;
	}

	int cnt = 0;
	unsigned tmp = 0;
	unsigned int *buf = &tmp;
	char output_name[256] = {0};
	char * pSuffix;
	
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("fopen file: %s, fail ! errno:0x%x, %s\n", argv[1], errno, strerror(errno));
		return -1;
	}

	strncpy(output_name, argv[1], sizeof(output_name) - sizeof(OUTPUT_SUFFIX));
	pSuffix = output_name + strlen(output_name);
	sprintf(pSuffix, OUTPUT_SUFFIX);
	
	FILE *out_fp = fopen(output_name, "w+");
	if (out_fp == NULL)	
	{
		fclose(fp);
		printf("fopen out file: %s, fail ! errno:0x%x, %s\n", output_name, errno, strerror(errno));
		return -1;
	}

	while(1){
		cnt = fread(buf, sizeof(unsigned int), 1, fp);
		if (cnt == 0){
			printf("reach file end, swap ok ! \n");
			break;
		} else if( cnt == -1){
			printf("read file:%s error, errno:0x%x, %s !", argv[1], errno, strerror(errno));
			break;
		}
		
		*buf = swap32(*buf);
		
		if (fwrite(buf, sizeof(unsigned int), cnt, out_fp) <= 0){
			printf("write out_fd fail! errno:0x%x, %s \n", errno, strerror(errno));
			break;
		}	
	}

	fclose(fp);
	fclose(out_fp);
	
	return 0;
}
