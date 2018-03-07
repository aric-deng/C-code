#ifndef __APP_FIFO_CLI_H__
#define __APP_FIFO_CLI_H__
#include "common_used.h"

#define READ_NAME "/tmp/client_read"
#define SEND_NAME "/tmp/client_send"

typedef struct _fifo_msg_t
{
	int msg_type;
	int sub_type; 
	int pack_count; 
	int pack_no;
	int ack;
	int session_id;
	int data_len;
	char reserv[4];
	char data_buf[16*1024];
}fifo_msg_t;

enum FIFO_MSG_TYPE
{
	FIFO_MSG_SYSTEM_ACTION = 0x01,
};

int open_read_fifo();
int open_write_fifo();
int read_fifo(int fd, fifo_msg_t *p_fifo_msg, int time_out_sec);
int write_fifo(int fd, const fifo_msg_t *p_fifo_msg, int time_out_sec);
int close_read_fifo(int fd);
int close_write_fifo(int fd);
int SGKS_SYSTEM(char *system_action, int timeout);

#endif
