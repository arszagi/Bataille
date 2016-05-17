//
// Created by Anthony Maton on 17/05/16.
//

#ifndef BATAILLE_CARD_H
#define BATAILLE_CARD_H

#include <math.h>

#define JACK    9
#define QUEEN   10
#define KING    11
#define AS      12

#define HEART   3
#define DIAMOND 2
#define CLUB    1
#define SPADE   0

#define BLACK   0
#define RED     1

#define HEART_SYMBOL "♥"
#define DIAMOND_SYMBOL "♦"
#define CLUB_SYMBOL  "♣"
#define SPADE_SYMBOL "♠"

void print_card(int card);

/*
 * Only accept RED or BLACK
 */
char * color_code(int color);

char * get_symbol(int symbol);

#endif //BATAILLE_CARD_H
