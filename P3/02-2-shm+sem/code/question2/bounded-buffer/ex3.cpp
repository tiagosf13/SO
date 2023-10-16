#include <stdlib.h>
#include <stdio.h>
#include "process.h"

#define ODD 1
#define EVEN 0

static int global_number;


int main(void)
{
    /* create the shared memory */
    int shmid = pshmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    int *pcounter = (int *)pshmat(shmid, NULL, 0);
    *pcounter = 1;

    // create the semaphores for the odd numbers and the even number
    int sems = psemget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    // ask the user for a number
    int input_number = -1;

    while (input_number < 10 || input_number > 20) {
        printf("Insert N1 value (10-20): ");
        scanf("%d", &input_number);
    }

    global_number = input_number;

    if(global_number%2 == 0)
    {
        psem_up(sems, EVEN); //up the semaphore of the first number to be printed
    }
    else{
        psem_up(sems, ODD);
    }
    

    pid_t pid1 = pfork();

    if (pid1 == 0)
    {
        while(*pcounter < input_number)
        {
            psem_down(sems, EVEN);
            printf("(%d) %d\n", pid1, (*pcounter)++);
            psem_up(sems, ODD);
        }
    }
    pid_t pid2 = pfork();
    if (pid2 == 0)
    {
        while(*pcounter < input_number)
        {
            psem_down(sems, ODD);
            printf("(%d) %d\n", pid1, (*pcounter)++);
            psem_up(sems, EVEN);
        }
    }

    if(!(pid1 == 0 || pid2 ==0))
    {
        pwait(NULL);
        pwait(NULL);
    }

    return EXIT_SUCCESS;
}