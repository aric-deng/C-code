#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include "defines.h"

#define MAXLINE 4096


int nb_set_socket_onbio(int fd, int noBlock)
{
    if (ioctl(fd, FIONBIO, &noBlock) < 0) {
        err("ioctl FIONBIO err strerror:%s errno:%d\n", strerror(errno), errno);
        return -1;
    }
    return  0;
}

int nb_set_socket_linger(int fd)
{
    struct linger nLinger;

    if (fd <= 0)
    {
        return -1;
    }

    memset(&nLinger, 0, sizeof(struct linger));
    nLinger.l_onoff = 1;
    nLinger.l_linger = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (void *)&nLinger, sizeof(struct linger)) < 0)
    {
        err("SO_LINGER err strerror:%s errno:%d\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

int nb_set_socket_nodelay(int fd)
{
    int opt = 1;

    if (fd <= 0)
    {
        return -1;
    }

    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt)) < 0)
    {
        err("TCP_NODELAY err strerror:%s errno:%d\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

int nb_set_socket_keepalive(int fd)
{
    int opt = 1;

    if (fd <= 0)
    {
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) < 0)
    {
        err("TCP_NODELAY err strerror:%s errno:%d\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

int nb_set_socket_attr(int fd, int bReuseAddr, int nSndTimeO, int nRcvTimeO, int nSndBuf, int nRcvBuf)
{
    int err_ret = 0;
    struct timeval sndTo, rcvTo;

    if (fd <= 0)
    {
        return -1;
    }

    sndTo.tv_sec  = nSndTimeO / 1000;
    sndTo.tv_usec = (nSndTimeO % 1000) * 1000;

    rcvTo.tv_sec  = nRcvTimeO / 1000;
    rcvTo.tv_usec = (nRcvTimeO % 1000) * 1000;

    if (bReuseAddr != 0 && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&bReuseAddr, sizeof(int)) < 0)
    {
        err("SO_REUSEADDR err strerror:%s errno:%d\n", strerror(errno), errno);
        err_ret = -1;
    }
    if (nSndTimeO != 0 && setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&sndTo, sizeof(sndTo)) < 0)
    {
        err("SO_SNDTIMEO err strerror:%s errno:%d\n", strerror(errno), errno);
        err_ret = -1;
    }
    if (nSndTimeO != 0 && setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcvTo, sizeof(rcvTo)) < 0)
    {
        err("SO_RCVTIMEO err strerror:%s errno:%d\n", strerror(errno), errno);
        err_ret = -1;
    }
    if (nSndBuf != 0 && setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&nSndBuf, sizeof(nSndBuf)) < 0)
    {
        err("SO_SNDBUF err strerror:%s errno:%d\n", strerror(errno), errno);
        err_ret = -1;
    }
    if (nRcvBuf != 0 && setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&nRcvBuf, sizeof(nSndBuf)) < 0)
    {
        err("SO_RCVBUF err strerror:%s errno:%d\n", strerror(errno), errno);
        err_ret = -1;
    }

    return err_ret;
}

int nb_set_tcp_option(int sockfd)
{
    nb_set_socket_nodelay(sockfd);
    nb_set_socket_keepalive(sockfd);
    nb_set_socket_attr(sockfd, 1, 0, 0, 0, 0);

    return 0;
}

/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
 
int main(int argc, char** argv)
{
    int    sockfd, n;
	char 	tmpChar;
    char    recvline[4096], sendline[4096];
    struct sockaddr_in    servaddr;

    if (argc != 2)
	{
		printf("usage: ./client <ipaddress>\n");
		exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		exit(0);
    }

	//nb_set_socket_onbio(sockfd, 1);
    //nb_set_socket_linger(sockfd);
    nb_set_socket_nodelay(sockfd);
    nb_set_socket_keepalive(sockfd);
    nb_set_socket_attr(sockfd, 1, 0, 0, 0, 0);
	
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
	{
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
    }
	
	while (1)
	{
		printf("send msg to server:\n");
		//tmpChar = fgetc(stdin);
		tmpChar = getch();
		printf("getch: %c \n", tmpChar);
		if (send(sockfd, &tmpChar, sizeof(tmpChar), 0) < 0)
		{
			err("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			close(sockfd);
			exit(0);
		}
		
	#if 0
		fgets(sendline, 4096, stdin);
		if (send(sockfd, sendline, strlen(sendline), 0) < 0)
		{
			err("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			close(sockfd);
			exit(0);
		}

		if (strncmp(sendline, "exit", 4) == 0)
		{
			break;
		}
	#endif
	}

    close(sockfd);
    exit(0);
}

