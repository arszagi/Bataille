/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#ifndef BATAILLE_GAME_H
#define BATAILLE_GAME_H

#include "constants.h"
#include "shared_memory.h"

typedef enum phase {
    REGISTRATION,
    PLAY
} Phase;

typedef struct player {
    /* TODO check char * != or === char */
    char * name[8];
    User user;
    int socket;
} Player;

typedef struct game {
    Phase phase;
    Player players[MAX_PLAYERS];
    int player_count;
} Game;

#endif //BATAILLE_GAME_H
