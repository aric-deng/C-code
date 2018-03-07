
#include <sys/time.h>

#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "app_fifo.h"
#include "app_loglib.h"


static int g_session_id = 0;

int open_read_fifo()
{
    int fd;
    int res;
    if (access(READ_NAME, F_OK) == -1)
    {
        res = mkfifo(READ_NAME, 0777);
        if (res != 0)
        {
            fprintf(stderr, "Counldn't create fifo %s!\n", READ_NAME);
            return(-1);
        }
    }
    fd = open(READ_NAME, O_RDWR);
    if (fd == -1)
    {
        fprintf(stderr, "Counldn't open fifo %s!\n", READ_NAME);
        return(-1);
    }
    return fd;
}

int open_write_fifo()
{
    int fd;
    int res;
    if (access(SEND_NAME, F_OK) == -1)
    {
        res = mkfifo(SEND_NAME, 0777);
        if (res != 0)
        {
            fprintf(stderr, "Counldn't create fifo %s!\n", SEND_NAME);
            return(-1);
        }
    }
    fd = open(SEND_NAME, O_RDWR);
    if (fd == -1)
    {
        fprintf(stderr, "Counldn't open fifo %s!\n", SEND_NAME);
        return(-1);
    }
    return fd;
}

int read_fifo(int fd, fifo_msg_t *p_fifo_msg, int time_out_sec)
{
    fd_set fds;
    struct timeval timeout;
    timeout.tv_sec = time_out_sec;

    fifo_msg_t s_read_fifo_msg;
    memset(&s_read_fifo_msg, 0, sizeof(fifo_msg_t));
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    switch(select(fd + 1, &fds, NULL, NULL, &timeout))
    {
    case 0:
    {
        ///dbgPrt("time out\n");
        return -1;
    }
    case -1:
    {
        return -1;
    }
    default:
    {
        if (FD_ISSET(fd, &fds))
        {
            /**< read data from server*/
            if (read(fd, &s_read_fifo_msg, sizeof(fifo_msg_t)) <= 0)
            {
                fprintf(stderr, "server read cmd error\n");
                return (-1);
            }
            *p_fifo_msg = s_read_fifo_msg;
        }
    }
    }
    return 0;
}

int write_fifo(int fd, const fifo_msg_t *p_fifo_msg, int time_out_sec)
{
    fd_set fds;
    struct timeval timeout;
    timeout.tv_sec = time_out_sec;

    fifo_msg_t s_write_fifo_msg;
    s_write_fifo_msg = *p_fifo_msg;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    switch(select(fd + 1, NULL, &fds, NULL, &timeout))
    {
    case 0:
    {
        ////dbgPrt("time out\n");
        return -1;
    }
    case -1:
    {
        return -1;
    }
    default:
    {
        if (FD_ISSET(fd, &fds))
        {
            /**< write data to server*/
            if (write(fd, &s_write_fifo_msg, sizeof(fifo_msg_t)) <= 0)
            {
                fprintf(stderr, "client write cmd error\n");
                return (-1);
            }
            fsync(fd);
            /////dbgPrt("write\n");
        }
    }
    }
    return 0;
}

int close_read_fifo(int fd)
{
    close(fd);
    return 0;
}

int close_write_fifo(int fd)
{
    close(fd);
    return 0;
}

int SGKS_SYSTEM(char *system_action, int timeout)
{
    if (NULL == system_action)
    {
        return -1;
    }
    if ((strcmp("reboot",  system_action)) == 0)
    {
        log_write(LOG_TYPE_SYSTEM, "system will restart!");
    }

    if (strstr(system_action, "hostapd") != NULL)
    {
        dbgPrt("\n\n");
        dbgPrt("host: %s\n",system_action);
        dbgPrt("\n\n");
    }

    fifo_msg_t s_read_fifo_msg;
    fifo_msg_t s_write_fifo_msg;
    int ret = -1;
    int write_fd = open_write_fifo();
    if (-1 == write_fd)
    {
        return -1;
    }
    g_session_id++;
    memset(&s_write_fifo_msg, 0, sizeof(fifo_msg_t));
    s_write_fifo_msg.msg_type = FIFO_MSG_SYSTEM_ACTION;
    s_write_fifo_msg.ack = 0;
    s_write_fifo_msg.session_id = g_session_id;
    strncpy(s_write_fifo_msg.data_buf, system_action, strlen(system_action) + 1);
    s_write_fifo_msg.data_len = strlen(system_action) + 1;

    /* write cmd_data to fifo server*/
    ret = write_fifo(write_fd, &s_write_fifo_msg, 20);//timeout
    if (-1 == ret)
    {
        fprintf(stderr, "SGKS_system write_fifo error1\n");
        g_session_id--;
        close_write_fifo(write_fd);
        return ret;
    }

    /* read data from server*/
    int read_fd = open_read_fifo();
    if (-1 == read_fd)
    {
        close_write_fifo(write_fd);
        g_session_id--;
        return -1;
    }
    memset(&s_read_fifo_msg, 0, sizeof(fifo_msg_t));
    ret = read_fifo(read_fd, &s_read_fifo_msg, 20);
    if (-1 == ret)
    {
        fprintf(stderr, "SGKS_system write_fifo error2\n");
    }
    g_session_id--;
    close_read_fifo(read_fd);
    close_write_fifo(write_fd);
    if (-1 == s_read_fifo_msg.ack)
    {
        return -1;
    }

    return ret;

}

#if 0

int main(void)
{
    fifo_msg_t msg;
    char s[32] = {0};
    while(1)
    {
        dbgPrt("input s:");
        memset(s, 0, sizeof(s));
        fgets(s, sizeof(s), stdin);
        s[strlen(s) - 1] = '\0';
        memset(&msg, 0, sizeof(fifo_msg_t));
        int ret = SGKS_SYSTEM(s);
        if (-1 == ret)
        {
            fprintf(stderr, "main error\n");
            //return -1;
        }
        dbgPrt("msg_data = %s\n", msg.data_buf);
    }
    return 0;
}

#endif
