//
// Created by Anthony Maton on 17/05/16.
//

#include <stdio.h>
#include "card.h"


char * format_card(int card) {
    char *colored_card;
    char *type_display; /* AS -> King */

    int value = card / 4;
    double symbols;
    modf(card / 4.0, &symbols);
    symbols *= 4;

    switch(value) {
        case AS:
            asprintf(&type_display, "%d", value + 1);
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
            asprintf(&type_display, "%d", value + 1);
            break;
        case JACK:
            asprintf(&type_display, "%s", "J");
            break;
        case QUEEN:
            asprintf(&type_display, "%s", "Q");
            break;
        case KING:
            asprintf(&type_display, "%s", "K");
            break;
    }

    switch((int) symbols) {
        case HEART:
            asprintf(&colored_card,"%s%d%s", color_code(RED), &type_display, "\033[0m");
            break;
        case DIAMOND:
            asprintf(&colored_card,"%s%d%s", color_code(RED), &type_display, "\033[0m");
            break;
        case CLUB:
            asprintf(&colored_card,"%s%d%s", color_code(BLACK), &type_display, "\033[0m");
            break;
        case SPADE:
            asprintf(&colored_card,"%s%d%s", color_code(BLACK), &type_display, "\033[0m");
            break;
    }

    return colored_card;
}

char * color_code(int color) {
    if(color == RED) {
        return "\033[0;31;47m";
    }else if(color == BLACK) {
        return "\033[0;30;47m";
    }
}


