/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#ifndef BATAILLE_CLIENT_H
#define BATAILLE_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "game.h"
#include "network.h"
#include "messages.h"


#define IP_ADDRESS 127.0.0.1

void argument_check(int argc, char ** argv);
void ask_pseudo();
void send_message(int code, int card);
Message read_message(int sd);
void send_card();
void remove_card(int card);
void catch_win_cards(Message win_card);
void lost_round(Message lost_message);

/*
 * Stockage des affichages du client
 */

char * welcome =
        "********************************* \n"
        "*** C'est l'heure de Bataille *** \n"
        "********************************* \n\n"
        "BIENVENUE !!!\n\n"
        "Quel est ton nom ?\n"
        "Nom [7 caractères maximum]: \n";

char * name_is_too_short = "Le nom est trop court (minimum 1 caractères).";
#endif //BATAILLE_CLIENT_H
