//
// Created by Anthony Maton on 17/05/16.
//

#ifndef BATAILLE_CARD_H
#define BATAILLE_CARD_H

#include <math.h>

#define AS      0
#define JACK    10
#define QUEEN   11
#define KING    12

#define HEART   0
#define DIAMOND 1
#define CLUB    2
#define SPADE   3

#define BLACK   0
#define RED     1

#define HEART_SYMBOL '♥'
#define DIAMOND_SYMBOL '♦'
#define CLUB_SYMBOL  '♣'
#define SPADE_SYMBOL '♠'

void print_card(int card);

/*
 * Only accept RED or BLACK
 */
char * color_code(int color);

char * get_symbol(int symbol);

#endif //BATAILLE_CARD_H

