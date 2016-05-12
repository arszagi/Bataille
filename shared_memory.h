//
// Created by Anthony Maton on 9/05/16.
//

#ifndef BATAILLE_SHARED_MEMORY_H
#define BATAILLE_SHARED_MEMORY_H

#include "constants.h"

#define FTOK_SEMAPHORES_ID 's'
#define FTOK_SHARED_MEMORY_ID 'm'

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

typedef struct player {
    char name[8];
    int score;
} Player;

typedef struct Scoreboard {
    Player players[MAX_PLAYERS];
} Scoreboard;

#endif //BATAILLE_SHARED_MEMORY_H
