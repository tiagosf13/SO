#include <stdlib.h>
#include <stdio.h>
#include "process.h"


int main(void)
{
    /* create the shared memory */
    int shmid = pshmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    int *pcounter = (int *)pshmat(shmid, NULL, 0);
    *pcounter = 1;

    pid_t pid = pfork();

    if (pid == 0){
        printf("Child process\n");

        // ask the user for a number
        int n;

        printf("Enter a number: ");
        scanf("%d", &n);

        for(; *pcounter<=n; (*pcounter)++){
            //usleep(1000000);
            printf("%d\n", *pcounter);
        }
        exit(0); // adicionar sempre exit no processo do filho
    }
    else{
        pwait(NULL); // Wait for child process to finish
        printf("Parent process\n");
        for(; *pcounter>=1; (*pcounter)--){
            //usleep(1000000);
            printf("%d\n", *pcounter);
        }
    }

    return EXIT_SUCCESS;

}