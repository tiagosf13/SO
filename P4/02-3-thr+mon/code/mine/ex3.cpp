#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

int global_number;
pthread_mutex_t mutex_thread1;
pthread_mutex_t mutex_thread2;

void* thread_main(void *arg)
{
    // get thread id
    int thread_pid = *((int*)arg);

    while(global_number>1)
    {
        if (thread_pid==1)
        {   
            /*  
                We lock thread1 to initiate the decrement
                and it will stay locked until thread2 has finished with its decrement,
                this way, thread1 will have no activity and therefore it wont decrement
            */
            pthread_mutex_lock(&mutex_thread1);
            printf("(Thread-%d) %d\n", thread_pid, global_number);
            global_number--;
            pthread_mutex_unlock(&mutex_thread2); // thread2 is unlocked and allowed to decrement
        }
        else{
            /*  
                We lock thread2 to initiate the decrement
                and it will stay locked until thread1 has finished with its decrement,
                this way, thread1 will have no activity and therefore it wont decrement
            */
            pthread_mutex_lock(&mutex_thread2);
            printf("(Thread-%d) %d\n", thread_pid, global_number);
            global_number--;
            pthread_mutex_unlock(&mutex_thread1); // thread1 is unlocked and allowed to decrement
        }
    }

    return NULL;
}


int main(void)
{
    int input_number = -1;
    pthread_t thread1;
    pthread_t thread2;

    while((input_number < 10) || (input_number>20))
    {
        printf("Insert N1 value (10-20):");
        scanf("%d", &input_number);
    }

    global_number = input_number;

    int pid_thread1 = 1; int pid_thread2 = 2;


    // initiate the mutex
    pthread_mutex_init(&mutex_thread1, NULL);
    pthread_mutex_init(&mutex_thread2, NULL);

    // create two threads to decrement
    pthread_create(&thread1, NULL, thread_main, &pid_thread1);
    pthread_create(&thread2, NULL, thread_main, &pid_thread2);

    //start the threads and wait until they are finished
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // destroy the mutex
    pthread_mutex_destroy(&mutex_thread1);
    pthread_mutex_destroy(&mutex_thread2);
    
    return EXIT_SUCCESS;
}