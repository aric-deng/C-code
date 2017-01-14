#include <stdio.h>
#include <signal.h>

int getPidFromName(char *pName)
{
	char buf[16] = {0}, cmdStr[16] = {0};
	sprintf(cmdStr, "pidof %s", pName);
	FILE *cmd = popen(cmdStr, "r");
	fgets(buf, sizeof(buf), cmd);
	pid_t pid = strtoul(buf, NULL, 10);
	pclose(cmd);

	return pid;
}

int main(void)
{
	union sigval mySigVal;
	mySigVal.sival_int = 0;
	if (sigqueue(getPidFromName("recv"), SIGUSR1, mySigVal) == -1) {
		perror("sigqueue error");
		return -1;
	}

	while (1) {
		sleep(3);
	}
	return 0;
}
