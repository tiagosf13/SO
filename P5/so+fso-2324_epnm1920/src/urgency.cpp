/**
 * @file
 *
 * \brief A hospital pediatric urgency with a Manchester triage system.
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <libgen.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/types.h>
#include  <thread.h>
#include  <math.h>
#include  <stdint.h>
#include  <signal.h>
#include  <utils.h>
#include <iostream>
#include  "settings.h"
#include  "pfifo.h"
#include <sys/types.h>
#include <unistd.h>

/* Changes made:
   - int main(int argc, char *argv[])
   - Added Module variables (Shared Memory ID, Patients Semaphores ID)
   - void term_simulation(int np)
   - int doctor_iteration(int id)
   - void patient_wait_end_of_consultation(int id)
   - void patient_life(int id)
*/

/* DO NOT CHANGE THE FOLLOWING VALUES, run program with option -h to set a different values */

static int npatients = 4;  ///< number of patients
static int nnurses = 1;    ///< number of triage nurses
static int ndoctors = 1;   ///< number of doctors


#define USAGE "Synopsis: %s [options]\n" \
   "\t----------+-------------------------------------------------------------\n" \
   "\t  Option  |          Description                                        \n" \
   "\t----------+-------------------------------------------------------------\n" \
   "\t -p num   | number of patients (dfl: %d, min: %d, max: %d)              \n" \
   "\t -n num   | number of nurses (dfl: %d, min: %d, max: %d)                \n" \
   "\t -d num   | number of doctors (dfl: %d, min: %d, max: %d)               \n" \
   "\t -h       | this help                                                   \n" \
   "\t----------+-------------------------------------------------------------\n", \
   basename(argv[0]), npatients, 1, MAX_PATIENTS, nnurses, 1, MAX_NURSES, ndoctors, 1, MAX_DOCTORS


// TODO point: changes may be required in these date structures

/**
 * \brief Patient data structure
 */
typedef struct
{
   char name[MAX_NAME+1];
   int done; // 0: waiting for consultation; 1: consultation finished
   // TODO point: if necessary, add new fields here
} Patient;

typedef struct
{
   int num_patients;
   Patient all_patients[MAX_PATIENTS];
   PriorityFIFO triage_queue;
   PriorityFIFO doctor_queue;
   // TODO point: if necessary, add new fields here
} HospitalData;

HospitalData * hd = NULL;

// TODO point: if necessary, add module variables here
static int shmid; // Shared Memory ID
static int patients_sems;  // The Patients Semaphores ID (for keeping the Patients until the end of consultation)


/**
 *  \brief verification tests:
 */
#define check_valid_patient(id) do { check_valid_patient_id(id); check_valid_name(hd->all_patients[id].name); } while(0)
#define check_valid_nurse(id) do { check_valid_nurse_id(id); } while(0)
#define check_valid_doctor(id) do { check_valid_doctor_id(id); } while(0)

int random_manchester_triage_priority();
void new_patient(Patient* patient); // initializes a new patient
void random_wait();


/* ************************************************* */

// TODO point: changes may be required to this function
void init_simulation(int np)
{
   /* When initiating the Simulation, I added the code to:
         1. Created np (Number of Patients) Semaphores to know when to discharge Patients from the consultation
         2. Created and Atached the HospitalData as Shared Memory, so that the processes access the same information
    */

   printf("Initializing simulation\n");

   // mem_alloc is a malloc with NULL pointer verification
   //hd = (HospitalData*)mem_alloc(sizeof(HospitalData));
   
   // Create np Semaphores (one for each Patient, to control who is good to go)
   patients_sems = psemget(IPC_PRIVATE, np, 0600 | IPC_CREAT | IPC_EXCL); // np Semaphores

   // Shared Memory Creation
   shmid = pshmget(IPC_PRIVATE, sizeof(HospitalData), 0600 | IPC_CREAT | IPC_EXCL);

   // Attach shm to pointer address:
   hd = (HospitalData*)pshmat(shmid, NULL, 0);

   memset(hd, 0, sizeof(HospitalData));
   hd->num_patients = np;
   
   init_pfifo(&hd->triage_queue);
   init_pfifo(&hd->doctor_queue);
}

/* ************************************************* */

// TODO point: changes may be required to this function
void term_simulation(int np) {
   /* When terminating the Simulation, I added the code to:
         1. Detach the Shared Memory
         2. Destroy the Shared Memory
    */

   // DO NOT WAIT THE TERMINATION OF ACTIVE ENTITIES IN THIS FUNCTION!
   // This function is just to release the allocated resources
   
   printf("Releasing resources\n");
   term_pfifo(&hd->doctor_queue);
   term_pfifo(&hd->triage_queue);
   //free(hd); as we dont use memalloc, we dont need to use free, we used pshmget and pshmat so we use pshmctl

   // Detach Shared Memory
   pshmdt((void*)hd);
   
   // Destroy Shared Memory
   pshmctl(shmid, IPC_RMID, NULL);
   hd = NULL;
}

/* ************************************************* */

// TODO point: changes may be required to this function
int nurse_iteration(int id) // return value can be used to request termination
{
   check_valid_nurse(id);
   printf("\e[34;01mNurse %d: get next patient\e[0m\n", id);
   int patient = retrieve_pfifo(&hd->triage_queue);
   check_valid_patient(patient);
   // TODO point: PUT YOUR NURSE TERMINATION CODE HERE:
   printf("\e[34;01mNurse %d: evaluate patient %d priority\e[0m\n", id, patient);
   int priority = random_manchester_triage_priority();
   printf("\e[34;01mNurse %d: add patient %d with priority %d to doctor queue\e[0m\n", id, patient, priority);
   insert_pfifo(&hd->doctor_queue, patient, priority);

   return 0;
}

/* ************************************************* */

// TODO point: changes may be required to this function
int doctor_iteration(int id) // return value can be used to request termination
{
   /* When the Doctor is working, I added the code to:
         1. When the Doctor ended treating the Patient, mark the Patient has Done.
         2. Increment the Patients Semaphore, so that the Patient knows when to leave.
    */

   check_valid_doctor(id);
   printf("\e[32;01mDoctor %d: get next patient\e[0m\n", id);
   int patient = retrieve_pfifo(&hd->doctor_queue);
   check_valid_patient(patient);

   // TODO point: PUT YOUR DOCTOR TERMINATION CODE HERE:
   printf("\e[32;01mDoctor %d: treat patient %d\e[0m\n", id, patient);
   random_wait();
   printf("\e[32;01mDoctor %d: patient %d treated\e[0m\n", id, patient);
   // TODO point: PUT YOUR PATIENT CONSULTATION FINISHED NOTIFICATION CODE HERE:

   // Mark the Patient has done so that it can be discharged
   hd->all_patients[patient].done = 1;

   // Increment the Patients Semaphore so that he knows he can leave the Hospital
   psem_up(patients_sems, patient);

   // Nota Minha
   // É aqui que devemos notificar o paciente de que a consulta terminou
   // Para isso, devemos fazer
   // lock(&mutex);
   // hd->all_patients[patient].done = 1;
   // unlock(&mutex);

   return 0;
}

/* ************************************************* */

void patient_goto_urgency(int id)
{
   new_patient(&hd->all_patients[id]);
   check_valid_name(hd->all_patients[id].name);
   printf("\e[30;01mPatient %s (number %d): get to hospital\e[0m\n", hd->all_patients[id].name, id);
   insert_pfifo(&hd->triage_queue, id, 1); // all elements in triage queue with the same priority!
}

// TODO point: changes may be required to this function
void patient_wait_end_of_consultation(int id)
{
   /* When the Doctor is working, I added the code to:
         1. Decremented the Patients Semaphore so that he knows he is in a consultation and he can only 
            leave when the Doctor says so (the Doctor will increment the same when good to go)
    */

   check_valid_name(hd->all_patients[id].name);
   // TODO point: PUT YOUR WAIT CODE FOR FINISHED CONSULTATION HERE:
   
   //Decrement the patient sem to indicate he is in a consultation
   psem_down(patients_sems, id);
   
   printf("\e[30;01mPatient %s (number %d): health problems treated\e[0m\n", hd->all_patients[id].name, id);
}

// TODO point: changes are required to this function
void patient_life(int id)
{
   /* When the Doctor is working, I added the code to:
         1. Commented the dummy code
    */
   
   patient_goto_urgency(id);
   //nurse_iteration(0);  // TODO point: to be commented/deleted in concurrent version
   //doctor_iteration(0); // TODO point: to be commented/deleted in concurrent version
   patient_wait_end_of_consultation(id);
   memset(&(hd->all_patients[id]), 0, sizeof(Patient)); // patient finished
}

/* ************************************************* */

// TODO point: if necessary, add extra functions here:

/* ************************************************* */

int main(int argc, char *argv[])
{
   /*
      When starting the program, I changed the code to:
         1. Commented the dummy code so that we can test it with ours
         2. Created the Active Entities
         3. Waited for all the patients, nurses and doctors processes to end
         4. Modified:
            - init_simulation(npatients) : In order to create the necessary structures to run the simulation
            - term_simulation(npatients) : To end the simulation with control and to prevent memory leaks
    */
   /* command line processing */
   int option;
   while ((option = getopt(argc, argv, "p:n:d:h")) != -1)
   {
      switch (option)
      {
         case 'p':
            npatients = atoi(optarg);
            if (npatients < 1 || npatients > MAX_PATIENTS)
            {
               fprintf(stderr, "Invalid number of patients!\n");
               return EXIT_FAILURE;
            }
            break;
         case 'n':
            nnurses = atoi(optarg);
            if (nnurses < 1 || nnurses > MAX_NURSES)
            {
               fprintf(stderr, "Invalid number of nurses!\n");
               return EXIT_FAILURE;
            }
            break;
         case 'd':
            ndoctors = atoi(optarg);
            if (ndoctors < 1 || nnurses > MAX_DOCTORS)
            {
               fprintf(stderr, "Invalid number of doctors!\n");
               return EXIT_FAILURE;
            }
            break;
         case 'h':
            printf(USAGE);
            return EXIT_SUCCESS;
         default:
            fprintf(stderr, "Non valid option!\n");
            fprintf(stderr, USAGE);
            return EXIT_FAILURE;
      }
   }

   /* start random generator */
   srand(getpid());

   /* init simulation */
   init_simulation(npatients);

   // TODO point: REPLACE THE FOLLOWING DUMMY CODE WITH code to launch
   // active entities and code to properly terminate the simulation.

   /* dummy code to show a very simple sequential behavior */

   /* for(int i = 0; i < npatients; i++)
   {
      printf("\n");
      random_wait(); // random wait for patience creation
      patient_life(i);
   } */
   /* end of dummy code */

   // Create the Active Entities
   for(int i = 0; i < npatients; i++)
   {
      pid_t p = pfork();
      if (p == 0) // Child process
      {
         patient_life(i);

         // When the patient was processed and treated, we end his life to indicate is good to go
         exit(0);
      }
   }

   for(int i = 0; i < nnurses; i++)
   {
      pid_t p = pfork();
      if (p == 0) // Child process
      {
         while (hd->triage_queue.cnt != 0)
         {
            nurse_iteration(i);
         }
         exit(0);
      }
   }

   for(int i = 0; i < ndoctors; i++)
   {
      pid_t p = pfork();
      if (p == 0) // Child process
      {
         while (hd->doctor_queue.cnt != 0)
         {
            doctor_iteration(i);
         }  
         // We only terminate the doctors when there are no more patients in the doctor
         exit(0);
      }
   }

   // Wait for Patients processes to end
   for(int i = 0; i < npatients; i++)
   {
      pwait(NULL);
   }

   // Wait for Nurses processes to end
   for(int i = 0; i < nnurses; i++)
   {
      pwait(NULL);
   }

   // Wait for Doctors processes to end 
   for(int i = 0; i < ndoctors; i++)
   {
      pwait(NULL);
   }

   /* terminate simulation */
   term_simulation(npatients);
   
   return EXIT_SUCCESS;
}


/* IGNORE THE FOLLOWING CODE */

int random_manchester_triage_priority()
{
   int result;
   int perc = (int)(100*(double)rand()/((double)RAND_MAX)); // in [0;100]
   if (perc < 10)
      result = RED;
   else if (perc < 30)
      result = ORANGE;
   else if (perc < 50)
      result = YELLOW;
   else if (perc < 75)
      result = GREEN;
   else
      result = BLUE;
   return result;
}

static char **names = (char *[]) {"Ana", "Miguel", "Luis", "Joao", "Artur", "Maria", "Luisa", "Mario", "Augusto", "Antonio", "Jose", "Alice", "Almerindo", "Gustavo", "Paulo", "Paula", NULL};

char* random_name()
{
   static int names_len = 0;
   if (names_len == 0)
   {
      for(names_len = 0; names[names_len] != NULL; names_len++)
         ;
   }

   return names[(int)(names_len*(double)rand()/((double)RAND_MAX+1))];
}

void new_patient(Patient* patient)
{
   strcpy(patient->name, random_name());
   patient->done = 0;
}

void random_wait()
{
   usleep((useconds_t)(MAX_WAIT*(double)rand()/(double)RAND_MAX));
}

