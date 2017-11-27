#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <aio.h>
#include <sys/time.h>

#define WSIZE 		(3 * 1024 * 1024)

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: %s [output_filename] !\n", argv[0]);
		return -1;
	}

	struct timeval	tv0,tv1;
	struct timezone	tz;

	int fd = open( argv[1], O_RDWR|O_CREAT );
	if(fd < 0)
	{
		printf("open file: %s, fail ! errno:0x%x, %s\n", argv[1], errno, strerror(errno));
		return -1;
	}
	
	struct aiocb my_aiocb;
	memset(&my_aiocb, 0, sizeof(struct aiocb));
	my_aiocb.aio_buf = malloc(WSIZE + 1);
	if (!my_aiocb.aio_buf) perror("malloc");
	
	my_aiocb.aio_fildes = fd;
	my_aiocb.aio_nbytes = WSIZE;
	
	int ret = aio_write( &my_aiocb );
	if(ret < 0)
	{
		printf("errno:0x%x, %s\n", errno, strerror(errno));
	}

	gettimeofday(&tv0,&tz);	
	printf("write start \n");
	while ( aio_error( &my_aiocb ) == EINPROGRESS ) ;
	gettimeofday(&tv1,&tz);
	printf("write needs %dms\n",(tv1.tv_sec*1000+tv1.tv_usec/1000)-(tv0.tv_sec*1000+tv0.tv_usec/1000));

	close(fd);
	chmod(argv[1], 00666);
	
	return 0;
}
