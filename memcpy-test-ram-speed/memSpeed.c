#include <stdio.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{

	char *psrc, *pdst;
	if(argc <2){
		printf("Usage:%s [data_len]\n", argv[0]);
		return -1;
	}

	long len = atoi(argv[1]);
	psrc = malloc(len);
	pdst = malloc(len);

	struct timeval	tv0,tv1;
	struct timezone	tz;

	gettimeofday(&tv0,&tz);
	memcpy(pdst, psrc, len);
	gettimeofday(&tv1,&tz);

	printf("write needs %dms\n",(tv1.tv_sec*1000+tv1.tv_usec/1000)-(tv0.tv_sec*1000+tv0.tv_usec/1000));

	return 0;
}
