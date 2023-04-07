//Safwat Khan
//113551208

#include <stdio.h> 

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <stdlib.h>

#include <unistd.h>

#include <stdbool.h>

#include <sys/sem.h> //used for semaphores

#include <semaphore.h>//used for semaphores

#define SHMKEY ((key_t) 7891) // Shared MEM Key
#define SEMKEY ((key_t) 400L) // Semaphore Key
#define NSEMS 1 // number of semaphores being created
/* P(s) - wait opearation (add 1 to semaphore)*/
#define signalP() semop(sem_id, P, 1)
/* V(s) - signal operation(decreament 1 from semaphore) */
#define waitV() semop(sem_id, V, 1)


typedef union  //This is the for Semaphore arguments
{
    int val;
    struct semid_ds *buf;
    short *array;

} semaunion;

int sem_id; //ID for semaphore
static struct sembuffer OP = { 0,-1,0 }; //semaphore buffer
static struct sembuffer OV = { 0,1,0 }; //semaphor buffer
struct sembuffer *P = &OP; //Pointer to above buffer
struct sembuffer *V = &OV; //Pointer to above buffer

// prototype for create().
void processOne(int *);
void processTwo(int *);
void processThree(int *);
void processFour(int *);

int main()
{
    int shmid; //id used for shared memory.
    //To store pid of four processes
    int pid1, pid2, pid3, pid4;
    int ID;
    int status;
    int test;

    /* Create and connect to a shared memory buffer*/
    int * total = 0;

    char *shmadd;
    shmadd = (char *)0;

    int semnum = 0;
    int value, value1;
    semaunion semctl_arg;
    semctl_arg.val = 1;
    // Create semaphores
    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget Error ");
        exit(1);
    }//creates shared memory with 666


    if ((total = (int *)shmat(shmid, shmadd, 0)) == -1)
    {
        perror("shmat error ");
        exit(0);
    }// attaches shared memory to main process and tests to see if it was sucessful
    sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666);
    if (sem_id < 0) printf("Error in creating the semaphore. ");

    // Initialize semaphore
    value1 = semctl(sem_id, semnum, SETVAL, semctl_arg);
    //Initialise shared memory
    *total = 0;
    //create child processes
    if ((pid1 = fork()) == 0)
    {//fork process#1 then run it
    processOne(total);
    shmdt(shmid);
    exit(0);
    }
    if ((pid1 != 0) && (pid2 = fork()) == 0)
    {//fork and run process#2
    processTwo(total);
    shmdt(shmid);
    exit(0);
    }
    if ((pid1 != 0) && (pid2 != 0) && (pid3 = fork()) == 0)
    {//fork and run process#3
    processThree(total);
    shmdt(shmid);
    exit(0);
    }
    if ((pid1 != 0) && (pid2 != 0) && (pid3 != 0) && (pid4 = fork()) == 0)
    {//fork and run process#4
    processFour(total);
    shmdt(shmid);
    exit(0);
    }
    // wait loop and release Shared memory
    wait();
    wait();
    wait();
    wait();

    printf(" ");
    printf("Child with ID: %d has just exited ", pid1);
    printf("Child with ID: %d has just exited ", pid2);
    printf("Child with ID: %d has just exited ", pid3);
    printf("Child with ID: %d has just exited ", pid4);


    if ((shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)//for cleaning up shared memory
    perror("shmctl Error ");
    exit(-1);
    }//error if unsuccessful


    value = semctl(sem_id, semnum, GETVAL, semctl_arg);
    if (value < 1) printf("Eror detected in SETVAL. ");
    // release shared memory
    semctl_arg.val = 0;
    status = semctl(sem_id, 0, IPC_RMID, semctl_arg);
    if (status < 0) printf("Error in releaseing the semaphore. ");


    printf(" ......................End of program......................... ");
    exit(0);//replaced return with exit to match

    void processOne(int *total)
    {
    int i = 0;
    int test = 0;

    for (i = 0; i<100000; i++)
    {
    signalP();//decrement semaphore value
    *total = *total + 1;
    waitV();//increment semaphore
    }
    printf("From process1: counter = %d. ", *total);
    }

    void processTwo(int *total)
    {
    int i = 0;

    for (i = 0; i<200000; i++)
    {
    signalP();//decrement semaphore value
    *total = *total + 1;
    waitV();//increment semaphore
    }
    printf("From process2: counter = %d. ", *total);

    }
    void processThree(int *total)
    {
        int i = 0;
        for (i = 0; i<300000; i++)
        {
            signalP();//decrement semaphore value
            *total = *total + 1;
            waitV();//increment semaphore
        }

        printf("From process3: counter = %d. ", *total);

    }
    void processFour(int *total)
    {
        int i = 0;
        for (i = 0; i<500000; i++)
        {
        signalP();//decrement semaphore value
        *total = *total + 1;
        waitV();//increment semaphore
        }

        printf("From process4: counter = %d. ", *total);

    }