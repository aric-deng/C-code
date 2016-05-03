#include <stdio.h>
#include <time.h>


int update_time(void)
{
	time_t tt;
	char buf[32];
	time(&tt);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tt));
	printf("dhs cur time:%s \n", buf);

	return 0;
}

int main(void)
{
	while (1)
	{
		update_time();
		usleep( 500 * 1000 );
	}

	return 0;
}
