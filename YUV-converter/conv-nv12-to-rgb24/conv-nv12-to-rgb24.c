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


void NV12_to_RGB24(unsigned int width , unsigned int height , unsigned char *yuyv , unsigned char *rgb)  
{  
	const int nv_start = width * height ;  
	unsigned int  i, j, index = 0, rgb_index = 0;  
	unsigned char y, u, v;  
	int r, g, b, nv_index = 0;	
	  
   
	for(i = 0; i <	height ; i++)  
	{  
		for(j = 0; j < width; j ++){  
			nv_index = i / 2  * width + j - j % 2;	
  
			y = yuyv[rgb_index];  
			u = yuyv[nv_start + nv_index + 1];	
			v = yuyv[nv_start + nv_index ];  
			  
		  
			r = y + (140 * (v-128))/100;  //r  
			g = y - (34 * (u-128))/100 - (71 * (v-128))/100; //g  
			b = y + (177 * (u-128))/100; //b  
				  
			if(r > 255)   r = 255;	
			if(g > 255)   g = 255;	
			if(b > 255)   b = 255;	
			if(r < 0)	  r = 0;  
			if(g < 0)	  g = 0;  
			if(b < 0)	  b = 0;  
			  
			index = rgb_index % width + (height - i - 1) * width;  
			rgb[index * 3+0] = b;  
			rgb[index * 3+1] = g;  
			rgb[index * 3+2] = r;  
			rgb_index++;  
		}  
	}  
	return 0;  
  
}  

int main(int argc, char *argv[])
{
	if (argc < 4){
		printf("Usage: %s [filename] [width] [height]!\n", argv[0]);
		return -1;
	}

	int cnt = 0, i = 0, j = 0;
	int width, height;
	unsigned char tmp = 0;
	char output_name[256] = {0};
	unsigned char *buf, *out_buf, *y, *u, *v;
	unsigned char *pout;
	U_UV16 *uv;
	char * pSuffix;
	
	int fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0){
		printf("open file: %s, fail !\n", argv[1]);
		return -1;
	}
		
	width = strtoul(argv[2], NULL, 0);
	height = strtoul(argv[3], NULL, 0);
	buf = malloc(sizeof(unsigned char) * width * height * 3/2);
	out_buf = malloc(sizeof(unsigned char) * width * height * 3);
	cnt = read(fd, buf, width * height * 3/2);
	if(cnt < width * height * 3/2){
		printf("read file length is wrong, len:%d\n", cnt);
		close(fd);
		return -1;
	}
	
	strncpy(output_name, argv[1], sizeof(output_name));
	pSuffix = rindex(output_name, '.');
	sprintf(pSuffix, ".rgb24");
		
	int out_fd = open(output_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (out_fd < 0){
		printf("open %s fail !\n", output_name);
		return -1;
	}
	
	NV12_to_RGB24(width, height, buf, out_buf);

	if (write(out_fd, out_buf, width * height * 3) <= 0){
		printf("write out_fd fail! \n");
	}	

	close(fd);
	close(out_fd);
	//free(buf);
	//free(out_buf);
	
	return 0;
}
