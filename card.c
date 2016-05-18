//
// Created by Anthony Maton on 17/05/16.
//

#include <stdio.h>
#include "card.h"
#include "game.h"


void print_card(int card) {
    char *colored_card;
    char *type_display; /* AS -> King */

    int value = card / 4;
    int symbols = (int) (((card / 4.0) - value) * 4);
    int color = symbols == 0 || symbols == 1 ? RED : BLACK;



    switch(value) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            printf("%s %d%s %s",color_code(color), value + 2, get_symbol(symbols), "\033[0m");
            break;
        case JACK:
            printf("%s %c%s %s", color_code(color), 'J', get_symbol(symbols), "\033[0m");
            break;
        case QUEEN:
            printf("%s %c%s %s", color_code(color), 'Q', get_symbol(symbols), "\033[0m");
            break;
        case KING:
            printf("%s %c%s %s", color_code(color), 'K', get_symbol(symbols), "\033[0m");
            break;
        case AS:
            printf("%s %d%s %s",color_code(color), 1, get_symbol(symbols), "\033[0m");
            break;
    }
}

#pragma clang diagnostic ignored "-Wreturn-type"
char * color_code(int color) {
    if(color == RED) {
        return "\033[0;31;47m";
    }else if(color == BLACK) {
        return "\033[0;30;47m";
    }
}

#pragma clang diagnostic ignored "-Wreturn-type"
char * get_symbol(int symbol){
    switch(symbol) {
        case HEART:
            return HEART_SYMBOL;
        case DIAMOND:
            return DIAMOND_SYMBOL;
        case CLUB:
            return CLUB_SYMBOL;
        case SPADE:
            return SPADE_SYMBOL;
    }
}

int compute_score(int * deck) {
    int i;
    int score = 0;
    for(i = 0; i < DECK_SIZE; i++){
        if (deck[i] == NO_CARD) {
            break;
        }
        switch(deck[i]) {
            case 48:
            case 49:
            case 50:
            case 51:
                score += 1;
                break;
            default:
                score += deck[i] + 2;
        }
    }
    return score;
}