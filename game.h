/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#ifndef BATAILLE_GAME_H
#define BATAILLE_GAME_H

#define NAME_SIZE 8
#define DECK_SIZE 52
#define NO_CARD -1

#include "constants.h"
#include "shared_memory.h"

int * fill_deck();

typedef enum phase {
    REGISTRATION,
    PLAY
} Phase;

typedef struct player {
    /* TODO check char * != or === char */
    char name[NAME_SIZE];
    User user;
    int socket;
    int hand[DECK_SIZE];
} Player;

typedef struct game {
    Phase phase;
    Player players[MAX_PLAYERS];
    int player_count;
} Game;



#endif //BATAILLE_GAME_H
