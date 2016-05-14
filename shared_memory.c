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
    char lock_file[MAX_ARRAY_SIZE];
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
    char lock_file[MAX_ARRAY_SIZE];
    key_t semaphore_key;

    sprintf(lock_file,"%s/bataille.lock", getpwuid(getuid())->pw_dir);

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

    if( (shmid=shmget(key, sizeof(Scoreboard), shmflg)) < 0)
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

/* Reader access to the memory management */

key_t get_reader_memory_token()
{
    char lock_file[MAX_ARRAY_SIZE];
    key_t memory_token;

    sprintf(lock_file,"%s/bataille.lock", getpwuid(getuid())->pw_dir);

    if( (memory_token=ftok(lock_file, FTOK_READER_MEMORY_ID)) == -1 )
    {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    return memory_token;
}

/**
 * Creates the shared 'reader' memory and returns his ID.
 */
int create_shared_reader_memory(int is_server)
{
    int reader_memory_id, shmflg;

    shmflg = (is_server) ? IPC_CREAT|0666 : 0666;

    if ((reader_memory_id = shmget(get_reader_memory_token(), sizeof(Scoreboard), shmflg)) < 0)
    {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    return reader_memory_id;
}

/**
 * Attach the shared reader memory.
 */
struct reader_memory* access_shared_reader_memory(int reader_memory_id)
{
    struct reader_memory *reader_memory_ptr;

    if( (reader_memory_ptr=shmat(reader_memory_id, NULL, 0)) == (void *)-1 )
    {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    return reader_memory_ptr;
}