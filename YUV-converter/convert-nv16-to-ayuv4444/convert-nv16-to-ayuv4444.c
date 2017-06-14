#include <stdio.h>
#include <fcntl.h>

typedef unsigned short u16;

typedef union ayuv {
  struct {
      u16 v :4;
      u16 u :4;
      u16 y :4;
      u16 a :4;
  } bit;
  u16 all;
} U_AYUV; 

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
	U_AYUV *pout;
	char * pSuffix;
	
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}
	
	strncpy(output_name, argv[1], sizeof(output_name));
	pSuffix = rindex(output_name, '.');
	sprintf(pSuffix, ".ayuv4444");
		
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
	pout = (U_AYUV *)out_buf;
	
	for(i = 0; i < height; i++)
		for (j = 0; j < width; j++){
			pout->bit.a = 0xF;
			pout->bit.y = *(y + (i + 1) * j) >> 4;
			if(j % 2 == 0){
				pout->bit.u = *(uv + i* width + j) >> 4;
				pout->bit.v = *(uv + i* width + j + 1) >> 4;
			} else {
				pout->bit.u = *(uv + i* width + j -1) >> 4;
				pout->bit.v = *(uv + i* width + j) >> 4;				
				
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
