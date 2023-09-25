#include <stdio.h>
#include <stdlib.h>
#include "process.h"

int main(){

    printf("Starting\n");

    pid_t pid = pfork();

    if (pid == 0){
        printf("Child process\n");
        for(int i=1; i<=10; i++){
            //usleep(1000000);
            printf("%d\n", i);
        }
    }
    else{
        pwait(NULL); // Wait for child process to finish
        printf("Parent process\n");
        for(int i=11; i<=20; i++){
            printf("%d\n", i);
        }
    }

    return EXIT_SUCCESS;
}