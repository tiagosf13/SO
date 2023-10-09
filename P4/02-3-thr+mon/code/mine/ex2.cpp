#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter;

void* thread_main(void*)
{

    int n2 = -1;
    while((n2 < 10) || (n2>20))
    {
        printf("Insert N2 value (10-20):");
        scanf("%d", &n2);
    }

    while(n2 != counter)
    {
        printf("(Thread) %d\n", ++counter);
    }
    
    return NULL;
}

int main(void)
{
    int n1 = -1;
    while((n1 < 1) || (n1>9))
    {
        printf("Insert N1 value (1-9):");
        scanf("%d", &n1);
    }

    counter = n1 - 1;

    pthread_t tid1;

    // Criar duas threads com o mesmo valor de n
    pthread_create(&tid1, NULL, thread_main, NULL);

    // Espere as duas threads terminarem
    pthread_join(tid1, NULL);
    counter--;
    while(counter>0)
    {
        printf("(Main) %d\n", counter--);
    }

    return EXIT_SUCCESS;
}
