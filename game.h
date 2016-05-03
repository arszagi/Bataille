//
// Created by Anthony Maton on 3/05/16.
//

#ifndef BATAILLE_GAME_H
#define BATAILLE_GAME_H

#include "constants.h"

typedef enum phase {
    REGISTRATION,
    PLAY
} Phase;

typedef struct player {
    char * name[8];
    int socket;
} Player;

typedef struct game {
    Phase phase;
    Player players[MAX_PLAYERS];
} Game;

#endif //BATAILLE_GAME_H
