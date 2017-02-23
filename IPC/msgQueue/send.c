#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>

#define MAX_TEXT 512

struct my_msg_st 
{
	long int my_msg_type;
	char some_text[MAX_TEXT];
};

int main()
{
	int running = 1;
	struct my_msg_st some_data;
	int msgid;
	char buffer[BUFSIZ];

	//int msgget(key_t key, int msgflg);
	//get msgQueue identifier associated with key, if no msgQueue with key exists, a new msgQueue is created according to IPC_CREAT
	//0666 is the same with file operation mode: rwx
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

	if (msgid == -1) 
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	while(running) 
	{
		printf("Enter some text: ");
		fgets(buffer, BUFSIZ, stdin);

		some_data.my_msg_type = 1;
		strcpy(some_data.some_text, buffer);


		//int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
		//append the msg data to msgQueue whose identifier is msgid
		if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) 
		{
			fprintf(stderr, "msgsnd failed\n");
			exit(EXIT_FAILURE);
		}

		if (strncmp(buffer, "end", 3) == 0) 
		{
			running = 0;
		}
	}
	
	exit(EXIT_SUCCESS);
}