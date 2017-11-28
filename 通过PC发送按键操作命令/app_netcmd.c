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

#include "common_used.h"
#include "ui_display.h"
#include "defines.h"
#include "app_misc.h"
#include "key_proc.h"
#include "app_netcmd.h"

#define MAXLINE 4096

int checkHexLegal(char c)
{
    /*
     * 0~9 || A~F || a~f
     */

    if (c >= 48 && c <= 57)
    {
        return c - '0';
    }
    else if (c >= 65 && c <= 70)
    {
        return c - 55;
    }
    else if (c >= 97 && c <= 102)
    {
        return c - 87;
    }
    else
    {
        return -1;
    }
}

int strToHex(char * str)
{
    int res = 0;
    int sum = 0;
    int i = 0;

    i = 0;
    while (1)
    {
        if ((res = checkHexLegal(str[i])) < 0)
        {
            break;
        }
        i += 1;
        sum = sum * 16 + res;
    }
    return sum;
}

int nb_set_socket_onbio(int fd, int noBlock)
{
    if (ioctl(fd, FIONBIO, &noBlock) < 0)
    {
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

void * app_netcmd_ServicePthread(void * arg)
{
    int    listenfd, connfd;
    struct sockaddr_in     servaddr;
    char    buff[4096];
    int     n;
    int cmd = 0;
    int param = 0;
    char * p = NULL;
    
    printf("\n----->>>>>> GK create Thread ok. %s  id = %d\n", __FUNCTION__, sysgettid());

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    //nb_set_socket_onbio(listenfd, 1);
    nb_set_socket_linger(listenfd);
    nb_set_socket_nodelay(listenfd);
    nb_set_socket_keepalive(listenfd);
    nb_set_socket_attr(listenfd, 1, 0, 0, 0, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(listenfd, 10) == -1)
    {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

#if 0
    while (1)
    {
        if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) == -1)
        {
            printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        printf("recv msg from client: %s\n", buff);
        close(connfd);
    }
#endif

RE_ACCEPT:
    if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) == -1)
    {
        err("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    while (1)
    {
        printf("======waiting for client's request======\n");
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        if (strncmp(buff, "exit", 4) == 0)
        {
            printf("exit\n");
            break;
        }

        if (n == 0)
        {
            close(connfd);
            connfd = -1;
            goto RE_ACCEPT;
        }
        printf("buff:0x%2x, %c\n", buff[0], buff[0]);

    #if 0
        cmd = strToHex(buff);
        if ((p = strstr(buff, "##")))
        {
            p += 2;
            //param = atoi(p);
            param = strToHex(p);
        }
        else
        {
            param = 255;
        }
        
        printf("recv msg from client: cmd:%02x param:%02x\n", cmd, param);
    #endif
        
        switch (buff[0])
        {
            case 'j':
                //short up
                //gk_key_handler(KEY_SHORT_DOWN | AUDIO_KEY_DOWN);
                gk_key_handler(0x10 | 0x02);
                break;
            case 'k':
                //short down
                //gk_key_handler(KEY_SHORT_DOWN | LOCK_KEY_DOWN);
                gk_key_handler(0x10 | 0x04);
                break;
            case 'h':
                //long up 
                gk_key_handler(0x20 | 0x02);
                break;
            case 'l':
                //long down
                gk_key_handler(0x20 | 0x04);
                break;
            case 'y':
                //short ok
                //gk_key_handler(KEY_SHORT_DOWN | REC_KEY_DOWN);
                gk_key_handler(0x10 | 0x01);
                break;
            case 'o':
                //long ok
                //gk_key_handler(KEY_SHORT_DOWN | REC_KEY_DOWN);
                gk_key_handler(0x20 | 0x01);
                break;    
            default:
                break;
        }
    }

    close(connfd);
    close(listenfd);
}

int NetCmd_SetPthreadStackSize(pthread_attr_t * attr, int stacksize)
{
    int pret = -1;

    pret = pthread_attr_init(attr);
    if (pret != 0)
    {
        return -1;
    }

    pret = pthread_attr_setstacksize(attr, stacksize);
    if (pret != 0)
    {
        return -1;
    }

    return 0;
}

int NetCmd_startService(void)
{
    pthread_t pid;
    pthread_attr_t attr;

    NetCmd_SetPthreadStackSize(&attr, 524288 * 2);
    int ret = pthread_create(&pid, &attr, app_netcmd_ServicePthread, NULL);
    if (ret < 0)
    {
        err("wifi_ServicePthread err!\n");
        return -1;
    }

    return 0;
}

