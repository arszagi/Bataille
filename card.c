//
// Created by Anthony Maton on 17/05/16.
//

#include <stdio.h>
#include "card.h"


void print_card(int card) {
    char *colored_card;
    char *type_display; /* AS -> King */

    int value = card / 4;
    int symbols = (int) (((card / 4.0) - value) * 4);
    int color = symbols == 0 || symbols == 1 ? RED : BLACK;



    switch(value) {
        case AS:
            printf("%s %d%s %s",color_code(color), value + 1, get_symbol(value), "\033[0m");
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            printf("%s %d%s %s", color_code(color), value + 1, get_symbol(symbols), "\033[0m");
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
    }
}

char * color_code(int color) {
    if(color == RED) {
        return "\033[0;31;47m";
    }else if(color == BLACK) {
        return "\033[0;30;47m";
    }
}

char * get_symbol(int symbol){
    switch(symbol) {
        case HEART:
            return "♥";
        case DIAMOND:
            return "♦";
        case CLUB:
            return "♣";
        case SPADE:
            return "♠";
    }
}