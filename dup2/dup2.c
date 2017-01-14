#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd2;
	int i = 0;

	int fd = open("./1.txt", O_CREAT | O_RDWR | O_APPEND, 00777);
	if (fd < 0){
		printf("open fail!\n");
		return -1;
	}

	fd2 = dup2(fd, STDOUT_FILENO);
	if (fd2 < 0){
		printf("dup fail. \n");
		close(fd);
		return -1;
	}
	close(fd);

	setbuf(stdout, NULL);
	for (; i<10; )
	{
		printf("%d \n", i++);
		sleep(1);
	}

	return 0;

}
