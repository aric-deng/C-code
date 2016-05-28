#include <stdio.h> 
#include <pthread.h> 
#include <unistd.h> 

pthread_mutex_t mutex; 
pthread_cond_t cond; 

void th1(void* args)
{
	int cnt=0;
	while (1){
		sleep(1);
		pthread_mutex_lock(&mutex);
		cnt++;
		printf("%s in, mutex lock cnt:%d \n", __func__, cnt);
		sleep(1);
		pthread_mutex_unlock(&mutex);
		printf("%s in, mutex unlock cnt:%d \n", __func__, cnt);
	}
}

void th2 (void * args)
{
	int cnt=0;
	while (1){
		sleep(1);
		if (pthread_mutex_trylock(&mutex)){
			printf("%s, mutex is already locked !! \n", __func__);
			continue;
		}
		cnt++;
		printf("%s in, mutex lock cnt:%d \n", __func__, cnt);
		sleep(1);
		pthread_mutex_unlock(&mutex);
		printf("%s in, mutex unlock cnt:%d \n", __func__, cnt);
	}
}

int main(void)
{
	int tid1, tid2;
	
	pthread_mutex_init(&mutex, NULL); 
	//pthread_cond_init(&cond,NULL); 
	pthread_create(&tid1,NULL,th1,NULL); 
	pthread_create(&tid2,NULL,th2,NULL); 
	
	while (1) {
		sleep(5);
		printf("main thread\n");
	}
	
	return 0;
}