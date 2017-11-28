#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

int main_run = 0;

int main(int argc, char *argv[])
{
	NetCmd_startService();
	
	main_run = 1;
	while (main_run)
	{
		sleep(5);
	}
	return 0;
}