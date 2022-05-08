/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2022 prestyan <shaoyang@nuaa.edu.cn>
 * 
 * signal_job9 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * signal_job9 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define bsize 4
#define item_count bsize*3

int buffer1[bsize];
int buffer2[bsize];
int in1,out1;
int in2,out2;

typedef struct
{
	int value;
	pthread_mutex_t mutex;//信号量为原子操作
	pthread_cond_t cond;
}sema_t;

sema_t mutex1,mutex2;
sema_t wait_empty_buff1;
sema_t wait_full_buff1;
sema_t wait_empty_buff2;
sema_t wait_full_buff2;

void put_item(int index,int item);
int get_item(int index);

void sema_wait(sema_t *sema);
void sema_signal(sema_t *sema);

void *produce ();
void *compute ();
void *consume ();

int main()
{
	init_sema ();
	pthread_t pid[3];
	pthread_create(pid,NULL,produce,NULL);
	pthread_create(pid+1,NULL,compute,NULL);
	pthread_create(pid+2,NULL,consume,NULL);

	for(int i=0;i<3;i++)
		pthread_join(pid[i],NULL);
	printf("---------END\n");
	return (0);
}

void init_sema()
{
	mutex1.value=1;
	pthread_mutex_init(&mutex1.mutex,NULL);
	pthread_cond_init(&mutex1.mutex,NULL);

	mutex2.value=1;
	pthread_mutex_init(&mutex2.mutex,NULL);
	pthread_cond_init(&mutex2.mutex,NULL);

	wait_empty_buff1.value=bsize-1;
	pthread_mutex_init(&wait_empty_buff1.mutex,NULL);
	pthread_mutex_init(&wait_empty_buff1.cond,NULL);

	wait_empty_buff2.value=bsize-1;
	pthread_mutex_init(&wait_empty_buff2.mutex,NULL);
	pthread_mutex_init(&wait_empty_buff2.cond,NULL);

	wait_full_buff1.value=0;
	pthread_mutex_init(&wait_full_buff1.mutex,NULL);
	pthread_mutex_init(&wait_full_buff1.cond,NULL);

	wait_full_buff2.value=0;
	pthread_mutex_init(&wait_full_buff2.mutex,NULL);
	pthread_mutex_init(&wait_full_buff2.cond,NULL);
}

void put_item(int index,int item)
{
	if(index==1)
	{
		buffer1[in1]=item;
		in1=(in1+1)%bsize;
	}
	else if(index==2)
	{
		buffer2[in2]=item;
		in2=(in2+1)%bsize;
	}
	else
	{
		perror("No such buffer.\n");
		return;
	}
}

int get_item(int index)
{
	int item;
	if(index==1)
	{
		item=buffer1[out1];
		out1=(out1+1)%bsize;
	}
	else if(index==2)
	{
		item=buffer2[out2];
		out2=(out2+1)%bsize;
	}
	else
	{
		perror ("No such buffer.\n");
		return 0;
	}
	return item;
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

void *produce()
{
	int item;
	for(int i=0;i<item_count;i++)
	{
		sema_wait (&wait_empty_buff1);
		sema_wait (&mutex1);
		item='a'+i;
		put_item(1,item);
		printf("Produce item: %c\n",item);
		sema_signal (&mutex1);
		sema_signal (&wait_full_buff1);
	}
	return NULL;
}

void *compute()
{
	int item,item_res;
	for(int i=0;i<item_count;i++)
	{
		sema_wait (&wait_full_buff1);
		sema_wait (&mutex1);
		item=get_item(1);
		sema_signal (&mutex1);
		sema_signal (&wait_empty_buff1);
		
		item_res=item-('a'-'A');
		printf("Compute item: %c-->%c\n",item,item_res);
		
		sema_wait (&wait_empty_buff2);
		sema_wait (&mutex2);
		put_item(2,item_res);
		sema_signal (&mutex2);
		sema_signal (&wait_full_buff2);
	}
	return NULL;
}

void *consume()
{
	int item;
	for(int i=0;i<item_count;i++)
	{
		sema_wait (&wait_full_buff2);
		sema_wait (&mutex2);
		item=get_item(2);
		printf("Consume item: %c\n",item);
		sema_signal (&mutex2);
		sema_signal (&wait_empty_buff2);
	}
	return NULL;
}
