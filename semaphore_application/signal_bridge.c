//Codes Structure
//test in win10-OS,vscode
/*
semaphore b=1,s=1,n=1;
int s2n=0,n2s=0;
ps2n()
{
    P(s);
    if(s2n==0){P(b);}
    s2n++;
    V(s);
    corss bridge;
    P(s);
    s2n--;
    if(n2s==0){V(b);}
    V(s);
}
ps2n()
{
    P(n);
    if(n2s==0){P(b);}
    n2s++;
    V(n);
    corss bridge;
    P(n);
    n2s--;
    if(n2s==0){V(b);}
    V(n);
}
*/
#include<stdio.h>
#include<Windows.h>
#include<process.h>
//两边各自过桥的人数（如果定义-1，表示无穷）
#define total_people 100
//线程句柄
HANDLE s2n[total_people],n2s[total_people];
//信号量句柄
HANDLE enter_bridge,use_s2n_people,use_n2s_people;
//上桥的人数
int s2n_people,n2s_people;
//线程函数
void pthread_SouthToNorth(void *arg);
void pthread_NorthToSouth(void *arg);

int main()
{
    //初始化
    s2n_people=0;
    n2s_people=0;
    enter_bridge=CreateSemaphore(NULL,1,1,NULL);
    use_s2n_people=CreateSemaphore(NULL,1,1,NULL);
    use_n2s_people=CreateSemaphore(NULL,1,1,NULL);
    printf("Total_people: %d\n",total_people);

    int num=(total_people==-1)?1:total_people;
    for(int i=0;i<num;i++)
    {
        s2n[i]=(HANDLE)_beginthread(pthread_SouthToNorth,32,NULL);
        n2s[i]=(HANDLE)_beginthread(pthread_NorthToSouth,32,NULL);
        //过桥穿插执行，需要一定随机性
        //r=rand();srand(time);Sleep((int)r*990/32767)
    }
    for(int i=0;i<num;i++)
    {
        WaitForSingleObject(s2n[i],INFINITE);
        WaitForSingleObject(n2s[i],INFINITE);
    }
    return 0;
}

void pthread_SouthToNorth(void *arg)
{
    do
    {
        WaitForSingleObject(use_s2n_people,INFINITE);
        if(s2n_people==0)
        {
            WaitForSingleObject(enter_bridge,INFINITE);
            s2n_people++;
        }
        else
            s2n_people++;
        ReleaseSemaphore(use_s2n_people,1,NULL);
        printf("%s\n","-->1 people enter bridge from south to north.");
        //cross bridge
        WaitForSingleObject(use_s2n_people,INFINITE);
        s2n_people--;
        if(s2n_people==0)
        {
            ReleaseSemaphore(enter_bridge,1,NULL);
            printf("%s\n","-->Last people leave bridge from south to north.");
        }
        ReleaseSemaphore(use_s2n_people,1,NULL);
        Sleep(1000);
    } while (total_people==-1);
    
    
}

void pthread_NorthToSouth(void *arg)
{
    do
    {
        WaitForSingleObject(use_n2s_people,INFINITE);
        if(n2s_people==0)
        {
            WaitForSingleObject(enter_bridge,INFINITE);
            n2s_people++;
        }
        else
            n2s_people++;
        ReleaseSemaphore(use_n2s_people,1,NULL);
        printf("%s\n","-->1 people enter bridge from north to south.");
        //cross bridge
        WaitForSingleObject(use_n2s_people,INFINITE);
        n2s_people--;
        if(n2s_people==0)
        {
            ReleaseSemaphore(enter_bridge,1,NULL);
            printf("%s\n","-->Last people leave bridge from north to south.");
        }
        ReleaseSemaphore(use_n2s_people,1,NULL);
        Sleep(1000);
    } while (total_people==-1);
} 