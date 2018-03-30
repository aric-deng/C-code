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

typedef union uv16 {
  struct {
	  u16 u :8;
      u16 v :8;
  } bit;
  u16 all;
} U_UV16; 

int main(int argc, char *argv[])
{
	if (argc < 4){
		printf("Usage: %s [filename] [width] [height]!\n", argv[0]);
		return -1;
	}

	int cnt = 0, i = 0, j = 0;
	int width = atoi(argv[2]), height = atoi(argv[3]);
	unsigned char tmp = 0;
	char output_name[256] = {0};
	unsigned char *buf, *out_buf, *y, *u, *v;
	unsigned char *pout;
	U_UV16 *uv, *uv_last;
	char * pSuffix;
	
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}
	
	strncpy(output_name, argv[1], sizeof(output_name));
	pSuffix = rindex(output_name, '.');
	sprintf(pSuffix, ".nv16");
		
	int out_fd = open(output_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (out_fd < 0){
		printf("open %s fail !\n", output_name);
		return -1;
	}
	
	buf = malloc(sizeof(unsigned char) * width * height * 3/2);
	out_buf = malloc(sizeof(unsigned char) * width * height * 2);
	cnt = read(fd, buf, width * height * 3/2);
	
	y = buf;
	u = buf + width * height;
	v = u + width * height / 4;
	pout = out_buf;
	
	memcpy(pout, y, width * height);
	pout += width * height;
	uv = (U_UV16 *)pout;
	
	for (i=0; i < height/4; i++)
	{
		uv_last = uv;
		for(j=0; j < width; j++){
			uv->bit.u = u[i*width+j];
			uv->bit.v = v[i*width+j];
			uv++;
		}

		memcpy(uv, uv_last, width);
		uv += width;
	}

	if (write(out_fd, out_buf, width * height * 2) <= 0){
		printf("write out_fd fail! \n");
	}	

	close(fd);
	close(out_fd);
	
	return 0;
}
