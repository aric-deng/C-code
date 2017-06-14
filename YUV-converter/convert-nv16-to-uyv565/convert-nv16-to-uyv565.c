#include <stdio.h>
#include <fcntl.h>

typedef unsigned short u16;

typedef union uyv565 {
  struct {
      u16 v :5;
      u16 y :6;
	  u16 u :5;
  } bit;
  u16 all;
} U_UYV565; 

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: %s [filename] !\n", argv[0]);
		return -1;
	}

	int cnt = 0, i = 0, j = 0;
	int width = 640, height = 480;
	unsigned char tmp = 0;
	char output_name[256] = {0};
	unsigned char *buf, *out_buf, *y, *uv;
	U_UYV565 *pout;
	char * pSuffix;
	
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}
	
	strncpy(output_name, argv[1], sizeof(output_name));
	pSuffix = rindex(output_name, '.');
	sprintf(pSuffix, ".uyv565");
		
	int out_fd = open(output_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (out_fd < 0){
		printf("open %s fail !\n", output_name);
		return -1;
	}
	
	buf = malloc(sizeof(unsigned char) * width * height * 2);
	out_buf = malloc(sizeof(unsigned char) * width * height * 2);
	cnt = read(fd, buf, width * height * 2);
	
	y = buf;
	uv = buf + width * height;
	pout = (U_UYV565 *)out_buf;
	
	for(i = 0; i < height; i++)
		for (j = 0; j < width; j++){
			pout->bit.y = *(y + i * width + j) >> 2;
			if(j % 2 == 0){
				pout->bit.u = *(uv + i* width + j) >> 3;
				pout->bit.v = *(uv + i* width + j + 1) >> 3;
			} else {
				pout->bit.u = *(uv + i* width + j -1) >> 3;
				pout->bit.v = *(uv + i* width + j) >> 3;				
				
			}
			pout++;
		}

	if (write(out_fd, out_buf, cnt) <= 0){
		printf("write out_fd fail! \n");
	}	

	close(fd);
	close(out_fd);
	
	return 0;
}
