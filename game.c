/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

int deck[DECK_SIZE];

/*Remplit le tableau avec des cartes et le melange ensuite.*/
int * fill_deck() {
    srand(time(NULL));
    int i, j, n = DECK_SIZE;
    for (i = 0; i < DECK_SIZE ; i++) {
        deck[i] = i;
    }
    int temp;
    for( i=0; i< n-1; i++) {
        // j is a random number between i and n-1 (included)
        j = i + rand() % (n-i);
        //swap the values of tab[i] and tab[j]
        temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    return deck;
}
