#include <stdlib.h>
#include <stdio.h>
#include "process.h"

#define ODD 0
#define EVEN 1


int main(void)
{
    /* create the shared memory */
    int shmid = pshmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    int *pcounter = (int *)pshmat(shmid, NULL, 0);
    *pcounter = 1;

    // create the semaphores for the odd numbers and the even number
    int sems = psemget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    // ask the user for a number
    int n;

    printf("Enter a number: ");
    scanf("%d", &n);


    psem_up(sems, !n%2); //up the semaphore of the first number to be printed


    pid_t pid = pfork();
    
    while( *pcounter < n) //se for <=n, um dos processos fica bloqueado
    {
        if (pid == 0) //print odds child
        {
            psem_down(sems, ODD);
            printf("(%d) %d\n", pid, (*pcounter)++);
            psem_up(sems, EVEN);
        }
        else //print even parent
        {
            psem_down(sems, EVEN);
            printf("(%d) %d\n", pid, (*pcounter)++);    
            psem_up(sems, ODD);
            
        }
        //printf("COUNTER %d - PID %d\n", *pcounter, pid);
    }
    return EXIT_SUCCESS;
}