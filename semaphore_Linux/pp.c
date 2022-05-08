#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define times 100

typedef struct
{
	int value;
	pthread_mutex_t mutex;//信号量为原子操作
	pthread_cond_t cond;
}sema_t;

sema_t wait_ping;
sema_t wait_pang;

void init();
void sema_wait(sema_t *sema);
void sema_signal(sema_t *sema);
void *pthread_ping();
void *pthread_pang();

int main()
{
	init();
	pthread_t pid[2];
	pthread_create(pid,NULL,pthread_ping,NULL);
	pthread_create(pid+1,NULL,pthread_pang,NULL);

	sema_signal (&wait_pang);//激活函数

	pthread_join(pid[0],NULL);
	pthread_join(pid[1],NULL);
	return 0;
}

void init_sema(sema_t *sema,int value)
{
	sema->value=value;
	pthread_mutex_init(&sema->mutex,NULL);
	pthread_cond_init(&sema->cond,NULL);
}
void init()
{
	init_sema (&wait_ping,0);
	init_sema (&wait_pang,0);
}

void sema_wait(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	sema->value--;
	while(sema->value<0)
		pthread_cond_wait(&sema->cond,&sema->mutex);
	pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	sema->value++;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}

void *pthread_ping()
{
	int i=0;
	do
	{
		sema_wait (&wait_pang);
		printf("Ping\n");
		sleep(1);
		sema_signal (&wait_ping);i++;
	}while(i<times);
	return NULL;
}

void *pthread_pang()
{
	int i=0;
	do
	{
		sema_wait(&wait_ping);
		printf("Pang\n");
		sleep(1);
		sema_signal (&wait_pang);i++;
	}while(i<times);
	return NULL;
}