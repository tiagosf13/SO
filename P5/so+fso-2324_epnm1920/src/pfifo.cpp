#include <dbc.h>
#include <string.h>
#include "pfifo.h"
#include "process.h"

#define FIFO_ACCESS 0
#define FIFO_SLOTS_AVAILABLE 1
#define FIFO_EMPTY 2

static void print_pfifo(PriorityFIFO* pfifo);
static int empty_pfifo(PriorityFIFO* pfifo);
static int full_pfifo(PriorityFIFO* pfifo);

// TODO point: initialization changes may be required in this function
void init_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");  // a false value indicates a program error

   // Create the 3 Semaphores
   pfifo->sem_id = psemget(IPC_PRIVATE, 3, 0600 | IPC_CREAT | IPC_EXCL); // 3 Semaphores

   // Set the semaphore FIFO_ITEMS to FIFO_MAXSIZE
   for (int i=0; i<FIFO_MAXSIZE; i++)
   {
      psem_up(pfifo->sem_id, FIFO_SLOTS_AVAILABLE);
   }

   //Up the FIFO_ACCESS semaphore
   psem_up(pfifo->sem_id, FIFO_ACCESS);

   memset(pfifo->array, 0, sizeof(pfifo->array));
   pfifo->inp = pfifo->out = pfifo->cnt = 0;
}

/* --------------------------------------- */

// TODO point: termination changes may be required in this function
void term_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");  // a false value indicates a program error

   // Destory the semaphores

   psemctl(pfifo->sem_id, FIFO_ACCESS, IPC_RMID, NULL);
   psemctl(pfifo->sem_id, FIFO_SLOTS_AVAILABLE, IPC_RMID, NULL);
   psemctl(pfifo->sem_id, FIFO_EMPTY, IPC_RMID, NULL);
}

/* --------------------------------------- */

// TODO point: synchronization changes may be required in this function
void insert_pfifo(PriorityFIFO* pfifo, int id, int priority)
{
   require (pfifo != NULL, "NULL pointer to FIFO");  // a false value indicates a program error
   
   //decrement the semaphore
   psem_down(pfifo->sem_id, 0);

   require ((id >= 0 && id <= MAX_ID) || id == DUMMY_ID, "invalid id");  // a false value indicates a program error
   require (priority > 0 && priority <= MAX_PRIORITY, "invalid priority value");  // a false value indicates a program error
   require (!full_pfifo(pfifo), "full FIFO");  // IMPORTANT: in a shared fifo, it may not result from a program error!

   


   // Nota Minha
   // Depois da linha 32, fazer o lock do mutex da fifo
   // Na linha 35, devemos fazer
   // while(fullp_fifo(pfifo)))
   //    pthread_cond_wait(&cond, &mutex);
   // Na linha 36, devemos fazer
   // Fazer as operações de inserção
   // pthread_broadcast(&cond);
   // pthread_mutex_unlock(&mutex);

   //printf("[insert_pfifo] value=%d, priority=%d, pfifo->inp=%d, pfifo->out=%d\n", id, priority, pfifo->inp, pfifo->out);

   int idx = pfifo->inp;
   int prev = (idx + FIFO_MAXSIZE - 1) % FIFO_MAXSIZE;
   while((idx != pfifo->out) && (pfifo->array[prev].priority > priority))
   {
      //printf("[insert_pfifo] idx=%d, prev=%d\n", idx, prev);
      pfifo->array[idx] = pfifo->array[prev];
      idx = prev;
      prev = (idx + FIFO_MAXSIZE - 1) % FIFO_MAXSIZE;
   }
   //printf("[insert_pfifo] idx=%d, prev=%d\n", idx, prev);
   pfifo->array[idx].id = id;
   pfifo->array[idx].priority = priority;
   pfifo->inp = (pfifo->inp + 1) % FIFO_MAXSIZE;
   pfifo->cnt++;
   //printf("[insert_pfifo] pfifo->inp=%d, pfifo->out=%d\n", pfifo->inp, pfifo->out);

   // UP the semaphore
   psem_up(pfifo->sem_id, 0);
}

/* --------------------------------------- */

// TODO point: synchronization changes may be required in this function
int retrieve_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");   // a false value indicates a program error
   require (!empty_pfifo(pfifo), "empty FIFO");       // IMPORTANT: in a shared fifo, it may not result from a program error!

   check_valid_patient_id(pfifo->array[pfifo->out].id);
   check_valid_priority(pfifo->array[pfifo->out].priority);

   int result = pfifo->array[pfifo->out].id;
   pfifo->array[pfifo->out].id = INVALID_ID;
   pfifo->array[pfifo->out].priority = INVALID_PRIORITY;
   pfifo->out = (pfifo->out + 1) % FIFO_MAXSIZE;
   pfifo->cnt--;

   // update priority of all remaing elements (increase priority by one)
   int idx = pfifo->out;
   for(int i = 1; i <= pfifo->cnt; i++)
   {
      if (pfifo->array[idx].priority > 1 && pfifo->array[idx].priority != INVALID_PRIORITY)
         pfifo->array[idx].priority--;
      idx = (idx + 1) % FIFO_MAXSIZE;
   }

   ensure ((result >= 0 && result <= MAX_ID) || result == DUMMY_ID, "invalid id");  // a false value indicates a program error

   return result;
}

/* --------------------------------------- */

static int empty_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");   // a false value indicates a program error

   return pfifo->cnt == 0;
}

/* --------------------------------------- */

static int full_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");   // a false value indicates a program error

   return pfifo->cnt == FIFO_MAXSIZE;
}

/* --------------------------------------- */

static void print_pfifo(PriorityFIFO* pfifo)
{
   require (pfifo != NULL, "NULL pointer to FIFO");   // a false value indicates a program error

   int idx = pfifo->out;
   for(int i = 1; i <= pfifo->cnt; i++)
   {
      check_valid_patient_id(pfifo->array[pfifo->out].id);
      check_valid_priority(pfifo->array[pfifo->out].priority);
      printf("[%02d] value = %d, priority = %d\n", i, pfifo->array[idx].id, pfifo->array[idx].priority);
      idx = (idx + 1) % FIFO_MAXSIZE;
   }
}

