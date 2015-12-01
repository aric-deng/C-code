#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/wait.h>

static int iPid=0;
int main(int argc, char* argv[])
{
	if(argc != 3 || argv[1] == NULL || argv[2] == NULL) {
		printf("dhs input args error, please check ! \n");
		return -1;
	}
	printf("dhs test in\n");
	 
	pid_t pid;
	pid = fork();
	switch(pid)
    {
        case -1:
            perror("Process Creation failed\n");
            return -1;
        case 0:
            printf("child process is running!\n");
            printf("My pid = %d,parentpid = %d\n",getpid(),getppid());
			
			//save current pid 
			iPid = getpid();  
			printf("dhs copying from %s to %s \n", argv[1], argv[2]);
			execl("/bin/cp", "cp", "-rf", argv[1], argv[2], NULL );

			printf("process never go to here!\n");
			return -1;
			
        default:
            printf("Parent process is running\n");
			sleep(2);
			int status;
		
			while (waitpid(pid,&status,0) < 0);
			
			if (WIFEXITED(status)) {
			   printf("exited, status=%d\n", WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
			   printf("killed by signal %d\n", WTERMSIG(status));
			} else if (WIFSTOPPED(status)) {
			   printf("stopped by signal %d\n", WSTOPSIG(status));
			}
		  
			break;
    }
	
	printf("dhs test out");
	return 0;
}
