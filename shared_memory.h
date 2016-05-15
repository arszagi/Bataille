//
// Created by Anthony Maton on 9/05/16.
//

#ifndef BATAILLE_SHARED_MEMORY_H
#define BATAILLE_SHARED_MEMORY_H

#include "constants.h"

#define MAX_ARRAY_SIZE 1024

#define FTOK_SEMAPHORES_ID 's'
#define FTOK_SHARED_MEMORY_ID 'm'
#define FTOK_READER_MEMORY_ID 'r'

#define SEMAPHORE_MUTEX 0
#define SEMAPHORE_ACCESS 1
#define SINGLE_OPERATION 1


void init_semaphore(int is_server);
key_t get_semaphore_token();
void delete_semaphores();
int semaphore_up(int semaphore);
int semaphore_down(int semaphore);
int create_shared_memory(int is_server);
struct Scoreboard *attach_memory(int shmid);

typedef struct line {
    char name[8];
    int score;
} Line;

typedef struct Scoreboard {
    Line players[MAX_PLAYERS];
} Scoreboard;

struct reader_memory {
    int reader_count;
};

key_t get_reader_memory_token();
int create_shared_reader_memory(int is_server);
struct reader_memory* access_shared_reader_memory(int reader_memory_id);

#endif //BATAILLE_SHARED_MEMORY_H
