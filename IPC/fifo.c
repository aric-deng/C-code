#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_NAME	"tmpFifo"

int main(int argc, char *argv[])
{
	char buf[80];
	int fd;
	unlink( FIFO_NAME );
	mkfifo( FIFO_NAME, 0777 );
	if ( fork() > 0 )
	{
		char s[] = "Hello World!\n";	
		fd = open( FIFO_NAME, O_WRONLY );
		if (argc < 2)
			write( fd, s, sizeof(s) );
		else
			write( fd, argv[1], strlen(argv[1]) );
		//close( fd );
	}
	else
	{
		fd = open( FIFO_NAME, O_RDONLY );
		read( fd, buf, sizeof(buf) );
		printf("The message from the pipe is:%s\n", buf );
		//close( fd );
	}
	
	close(fd);

	return 0;
}
