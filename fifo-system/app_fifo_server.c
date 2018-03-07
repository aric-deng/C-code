#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#define CMD_RESULT "/tmp/cmd_result"

#define READ_NAME "/tmp/client_send"

#define SEND_NAME "/tmp/client_read"



enum FIFO_MSG_TYPE
{
	FIFO_MSG_SYSTEM_ACTION = 0x01,
	FIFO_MSG_RESERV = 0xff	
};

typedef struct _fifo_msg_t
{
	int msg_type; //消息类型
	int sub_type; //子消息类型
	int pack_count;//当多个包返回时，可复用为pack_no
	int pack_no;
	int ack; //返回值
	int session_id; //确保会话唯一
	int data_len;
	char reserv[4];
	char data_buf[16*1024];
}fifo_msg_t;

int create_read_fifo(void)
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

int create_send_fifo(void)
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

int process_ret_file(char* data_buf, int size_buf)
{
    if (0 >= size_buf || NULL == data_buf)
    {
        return -1;
    }    
    int fd = open(CMD_RESULT, O_RDWR);
    if (-1 == fd)
    {
        fprintf(stderr, "open file error %s\n", CMD_RESULT);
        return -1;
    }
    int count = read(fd, data_buf, size_buf);
    if (0 > count)
    {
        fprintf(stderr, "read file error %s\n", CMD_RESULT);
        close(fd);
        return -1;    
    }
    else
    {
        data_buf[count] = '\0';
    }    
    close(fd);
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



int fifo_system(const char * cmdstring)
{
    pid_t pid;
    int status=0;
    int num =100;

    if(cmdstring == NULL)
    {
        return (1); //如果cmdstring为空，返回非零值，一般为1
    }

    if((pid = fork())<0)
    {
        status = -1; //fork失败，返回-1
    }
    else if(pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127); // exec执行失败返回127，注意exec只在失败时才返回现在的进程，成功的话现在的进程就不存在啦~~
    }
    else //父进程
    {
        if(waitpid(pid, &status, WNOHANG) <0)
        {

        }
    }
    //printf("fifo_system------------>\n");
    return status; //如果waitpid成功，则返回子进程的返回状态
}


int main ()
{
    fifo_msg_t s_read_fifo_msg;
    fifo_msg_t s_send_fifo_msg;    
    char    cmdline[128];
    char data_buf[16*1024] = {0};
    char cmd_str[16*1024] = {0};
    int fd_read;
    int fd_write;    
    int ret = -1;
    int process_ret = -1;
    fd_read = create_read_fifo();
    if (-1 == fd_read)
    {
        fprintf(stderr, "server:create_read_fifo error\n");
        return -1;
    }    
    fd_write = create_send_fifo();
    if (-1 == fd_write)
    {
        close_read_fifo(fd_read);
        fprintf(stderr, "server:create_send_fifo error\n");
        return -1;
    } 
    while(1)
    {
        memset(&s_read_fifo_msg, 0, sizeof(fifo_msg_t));
        
        /**< read data from client*/
        if (read(fd_read, &s_read_fifo_msg, sizeof(fifo_msg_t)) <= 0)
        {
            fprintf(stderr, "server read cmd error\n");
            //exit(-1);
        }
        switch(s_read_fifo_msg.msg_type)
        {
            case FIFO_MSG_SYSTEM_ACTION:
            {
                memset(cmdline, 0, sizeof(cmdline));
                memset(cmd_str, 0, sizeof(cmd_str));
                strncpy(cmdline, s_read_fifo_msg.data_buf, strlen(s_read_fifo_msg.data_buf));                    
                //printf("server read cmd_data :%s\n", cmdline);

				#if 0
				sprintf(cmd_str, "%s >& %s", cmdline, CMD_RESULT);
                
                ret = system(cmd_str);                
                memset(data_buf, 0, sizeof(data_buf));
                process_ret = process_ret_file(data_buf, sizeof(data_buf));/**< 得到system返回的数据*/

                if (-1 == process_ret)
                {
                    fprintf(stderr, "server process cmd error\n");
                    break;
                }
				#else
                signal(SIGCHLD,SIG_DFL);
				signal(SIGCHLD,SIG_IGN); 
				ret = fifo_system(cmdline);
				#endif

                memset(s_read_fifo_msg.data_buf, 0, sizeof(s_read_fifo_msg.data_buf));
                s_read_fifo_msg.ack = ret;
                memcpy(s_read_fifo_msg.data_buf, data_buf, strlen(data_buf) + 1);
               // strncpy(s_read_fifo_msg.data_buf, data_buf, strlen(data_buf) + 1);
                s_read_fifo_msg.data_len = strlen(data_buf) + 1;
                
                /**< 返回给客户端相应的数据*/                                
                if (write(fd_write, &s_read_fifo_msg, sizeof(fifo_msg_t)) <= 0)
                {
                    fprintf(stderr, "server write cmd error\n");    
                    //exit(-1);
                }
                //printf("write data :%s\n", data_buf);
                fsync(fd_write);                                                
                break;
            }
            default:
            {
                fprintf(stderr, "msg type is not exist\n", s_read_fifo_msg.msg_type);
                //return -1;
            } 
        }
        sleep(1);   
    }
    close_read_fifo(fd_read);
    close_write_fifo(fd_write);
    return 0;
}
