/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#ifndef BATAILLE_MESSAGES_H
#define BATAILLE_MESSAGES_H

#include "game.h"

#define CANCEL 0
#define REGISTER 1
#define INSCRIPTION_STATUS 2 //1 = OK  2 = NOK
#define DISTRIBUTION_CARDS 3
#define SEND_CARD 4
#define RETURN_WIN_CARDS 5
#define LAST_CARD 6 // derniere carte de la manche
#define END_ROUND 7
#define SEND_SCORE 8
#define END_GAME 9 // fin de la partie (serveur)


typedef struct Message Message;

union Payload {
    int number;
    int hand[DECK_SIZE];
    char name[8]; // Le nom fera maximum 8 caractères
};

struct Message {
    int type;
    union Payload payload;
};

// #define
#endif //BATAILLE_MESSAGES_H
