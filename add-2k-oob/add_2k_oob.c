#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define OUTPUT_SUFFIX "-oob"

#define ARRAY_LEN	(2048+64)
int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: %s [filename] !\n", argv[0]);
		return -1;
	}

	int cnt = 0;
	long long byte_cnt = 0;
	unsigned tmp = 0;
	unsigned int *buf = &tmp;
	char output_name[256] = {0};
	char * pSuffix;
	
	unsigned char array[ARRAY_LEN];
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("fopen file: %s, fail ! errno:0x%x, %s\n", argv[1], errno, strerror(errno));
		return -1;
	}

	memset(array, 0xFF, ARRAY_LEN);
	
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
			int temp = (byte_cnt * sizeof(unsigned int)) % 2048;
			if( temp != 0){
				//printf("not 2k align, cnt:%d, pad %d 0xFF to align. \n", temp, ARRAY_LEN-temp);
				if (fwrite(array, sizeof(unsigned char), ARRAY_LEN-temp, out_fp) <= 0)
				{
					printf("write out file: %s fail, errno:0x%x, %s \n", output_name, errno, strerror(errno));
					break;
				}	
			}
			//printf("reach file end, add oob ok ! \n");
			printf("add oob ok ! \n");
			break;
		} else if( cnt == -1){
			printf("read file:%s error, errno:0x%x, %s !", argv[1], errno, strerror(errno));
			break;
		}
		
		if (fwrite(buf, sizeof(unsigned int), cnt, out_fp) <= 0){
			printf("write out file: %s fail, errno:0x%x, %s \n", output_name, errno, strerror(errno));
			break;
		}	
		
		byte_cnt++;
		if(byte_cnt % (2048/sizeof(unsigned int)) == 0){
			if (fwrite(array, sizeof(unsigned char), 64, out_fp) <= 0)
			{
				printf("write out file: %s fail, errno:0x%x, %s \n", output_name, errno, strerror(errno));
				break;
			}	
		}
	}

	fclose(fp);
	fclose(out_fp);
	
	return 0;
}
