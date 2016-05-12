//
// Created by Anthony Maton on 9/05/16.
//

#include <stdlib.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/shm.h>
#include "shared_memory.h"

int semaphore_descriptor = -1;

key_t get_semaphore_token() {
    char lock_file[1024];
    key_t semaphore_key;

    sprintf(lock_file,"%s/bataille.lock", getpwuid(getuid())->pw_dir);

    if( (semaphore_key=ftok(lock_file, FTOK_SEMAPHORES_ID)) == -1 )
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    return semaphore_key;
}

/*
 * Initialise la sémaphore
 */
void init_semaphore(int is_server) {
    key_t semaphore_key = get_semaphore_token();
    int semaphore_flag = (is_server) ? IPC_CREAT|0666 : 0666;

    /* TODO Replace 1 by a Constant */
    if((semaphore_descriptor =  semget(semaphore_key, 1, semaphore_flag)) == -1){
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    if( semctl(semaphore_descriptor, SEMAPHORE_MUTEX, SETVAL, 1) == -1
        || semctl(semaphore_descriptor, SEMAPHORE_ACCESS, SETVAL, 1) == -1 )
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }
}

void delete_semaphores()
{
    if( semctl(semaphore_descriptor, 0, IPC_RMID)==-1 )
    {
        /* TODO Error management */
    }
    /* TODO Error management */
}

/*
 * Sortie de sémaphore
 */
int semaphore_up(int semaphore)
{
    struct sembuf operations_buffer[1], semaphore_operation;

    if( semaphore_descriptor == -1 )
    {
        /* TODO Error management */
        return -1;
    }

    semaphore_operation.sem_num = semaphore;
    semaphore_operation.sem_op = 1;
    semaphore_operation.sem_flg = 0;
    operations_buffer[0] = semaphore_operation;

    if( semop(semaphore_descriptor, operations_buffer, SINGLE_OPERATION) == -1 )
    {
        /* TODO Error management */
        return -1;
    }

    return 0;
}

/*
 * Entrée dans la sémaphore
 */
int semaphore_down(int semaphore)
{
    struct sembuf operations_buffer[1], semaphore_operation;

    if( semaphore_descriptor == -1 )
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    semaphore_operation.sem_num = semaphore;
    semaphore_operation.sem_op = -1;
    semaphore_operation.sem_flg = 0;
    operations_buffer[0] = semaphore_operation;

    if( semop(semaphore_descriptor, operations_buffer, SINGLE_OPERATION) == -1 )
    {
        /* TODO Error management */
        return -1;
    }

    return 0;
}

key_t get_shared_memory_token()
{
    char lock_file[1024];
    key_t semaphore_key;

    sprintf(lock_file,"%s/streams.lock", getpwuid(getuid())->pw_dir);

    if( (semaphore_key=ftok(lock_file, FTOK_SHARED_MEMORY_ID)) == -1 )
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    return semaphore_key;
}

int create_shared_memory(int is_server) {

    int shmid, shmflg;
    key_t key = get_shared_memory_token();

    shmflg = (is_server) ? IPC_CREAT|0666 : 0666;

    if( (shmid=shmget(key, sizeof(struct memory), shmflg)) < 0)
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    return shmid;
}

Scoreboard *attach_memory(int shmid) {

    Scoreboard* shared_mem_ptr;

    if( (shared_mem_ptr=shmat(shmid, NULL, 0)) == (void *)-1 )
    {
        /* TODO Error management */
        exit(EXIT_FAILURE);
    }

    return shared_mem_ptr;
}
