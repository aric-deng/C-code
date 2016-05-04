#include <stdio.h>
#include <signal.h>

void hello(int sig)
{
	printf("sig trigger \n");
}

int main(void)
{
	struct sigaction act;
	act.sa_handler = hello;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	int sig_no = SIGUSR1;
	if (sigaction(sig_no, &act, NULL) < 0){
		perror("sigaction");
	}
	
	while (1) {
		sleep(2);
	}

	return 0;
}
