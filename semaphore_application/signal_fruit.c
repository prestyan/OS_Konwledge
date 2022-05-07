//Codes Structure
//test in win10-OS,vscode
/*
father()
{
    P(plate);
    put orange;
    V(o);
}
mother()
{
    P(plate);
    put apple;
    V(a);
}
daughter(){P(o);get orange;V(put);}
son(){P(a);get apple;V(put);}
*/
#include<stdio.h>
#include<Windows.h>
#include<process.h>
#define plate_size 1

enum thing{ORANGE,APPLE,EMPTY};
enum thing plate;

//线程句柄
HANDLE ppo,ppa,pga,pgo;
//信号量句柄
HANDLE put,geta,geto;
//初始化函数
void initSemaphore(void);
//线程函数
void pthread_put_orange(void *arg);
void pthread_put_apple(void *arg);
void pthread_get_orange(void *arg);
void pthread_get_apple(void *arg);

int main()
{
    initSemaphore();
    //创建线程
    ppo=(HANDLE)_beginthread(pthread_put_orange,64,NULL);
    ppa=(HANDLE)_beginthread(pthread_put_apple,64,NULL);
    pgo=(HANDLE)_beginthread(pthread_get_orange,64,NULL);
    pga=(HANDLE)_beginthread(pthread_get_apple,64,NULL);
    //主线程等待
    WaitForSingleObject(ppo,10000);
    return 0;
}

void initSemaphore()
{
    printf("Plate Size: %d\n",plate_size);
    printf("Plate initial situation: Empty\n");
    printf("Main thread wait 10s.\n");
    put=CreateSemaphore(NULL,1,1,NULL);//初始有一个空盘子
    geta=CreateSemaphore(NULL,0,1,NULL);//没有apple，需要等待puta完毕
    geto=CreateSemaphore(NULL,0,1,NULL);//没有orange，需要等待puto完毕
    plate=EMPTY;
}

void pthread_put_orange(void *arg)
{
    while(1)
    {
        WaitForSingleObject(put,INFINITE);//P(put)
        plate=ORANGE;
        printf("%s\n","-->Father put 1 orange in plate.");
        ReleaseSemaphore(geto,1,NULL);//V(geto)
        Sleep(1000);
    }
}

void pthread_put_apple(void *arg)
{
    while(1)
    {
        WaitForSingleObject(put,INFINITE);//P(put)
        plate=APPLE;
        printf("%s\n","-->Mother put 1 apple in plate.");
        ReleaseSemaphore(geta,1,NULL);//V(geta)
        Sleep(1000);
    }
}

void pthread_get_orange(void *arg)
{
    while(1)
    {
        WaitForSingleObject(geto,INFINITE);//P(geto)
        plate=EMPTY;
        printf("%s\n","-->Daughter get 1 orange from plate.");
        ReleaseSemaphore(put,1,NULL);//V(put)
    }
}

void pthread_get_apple(void *arg)
{
    while(1)
    {
        WaitForSingleObject(geta,INFINITE);//P(geta)
        plate=EMPTY;
        printf("%s\n","-->Son get 1 apple from plate.");
        ReleaseSemaphore(put,1,NULL);//V(put)
    }
}