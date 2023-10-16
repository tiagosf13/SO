#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

static int global_number;
static pthread_mutex_t mutex_thread;

static pthread_cond_t cvar[2] = {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};

void* thread_main(void* arg) {
    //as variaveis de condição so podem funcionar dentro de um mutex
    int thread_id = *(int*)arg;
    pthread_mutex_lock(&mutex_thread);
    while (global_number > 1) {
        
        while (global_number %2 != thread_id)
        {
            cond_wait(&cvar[thread_id], &mutex_thread);
        }
        printf("(Thread-%d) global_number: %d\n",thread_id, global_number);
        global_number--;
        cond_broadcast(&cvar[global_number%2]);
    }
    pthread_mutex_unlock(&mutex_thread);
    return NULL;
}

int main(void) {
    int input_number = -1;
    pthread_t thread1;
    pthread_t thread2;

    while (input_number < 10 || input_number > 20) {
        printf("Insert N1 value (10-20): ");
        scanf("%d", &input_number);
    }

    global_number = input_number;

    // Initialize the mutex
    pthread_mutex_init(&mutex_thread, NULL);

    int pid_thread1 = 0, pid_thread2 = 1;

    // Create two threads to decrement alternately
    pthread_create(&thread1, NULL, thread_main, &pid_thread1);
    pthread_create(&thread2, NULL, thread_main, &pid_thread2);

    // Start the threads and wait until they are finished
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Destroy the mutex
    pthread_mutex_destroy(&mutex_thread);

    return EXIT_SUCCESS;
}

