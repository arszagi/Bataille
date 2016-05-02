/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 * Point d'entrée de l'application serveur et boucle principale du serveur.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include "utils.h"
#include "constants.h"
#include "network.h"
#include "messages.h"

void argument_check(int argc, char ** argv);
void register_signal_handlers();
// Equivalent to sig_end_handler(int signal_number)
void closing_handler(int signal_number);
Message read_message(int fd);

int server_fd;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char ** argv){

    int temp_fd;
    argument_check(argc, argv);
    server_fd = create_server(atoi(argv[1]), MAX_PLAYERS);
    fd_set file_descriptor_set;

    /*
     * Le serveur ne s'arrête jamais, nous avons donc recours à une boucle infinie
     * while(TRUE) et à des conditions de ruptures en cas d'erreur
     */
    while(TRUE){
        // Ici notre serveur doit tourner.

        // Nouvelle connexion :
        if(FD_ISSET(server_fd, &file_descriptor_set)){
            if((temp_fd = accept(server_fd, NULL, 0)) < 0){
                // TODO : Error management
                raise(SIGTERM);
            }
        }

        // Ecoute le message
    }
    return 0;
}

void argument_check(int argc, char ** argv){
    if(argc > 3 || argc < 2){
        fprintf(stderr, "Usage incorrect : \n\t "
                "Requiert au moins 1 argument : numéro de port\n\n"
                "Argument optionnel : fichier de sortie des messages\n\n"
                "USAGE : ./server PORT [Sortie]");
        exit(EXIT_FAILURE);
    }
    if(!isNumeric(argv[1])){
        fprintf(stderr, "Le numéro de port doit être un entier \n");
        exit(EXIT_FAILURE);
    }
}

void register_signal_handlers(){
    if(signal(SIGINT, closing_handler) == SIG_ERR) {
        // TODO : Error management
        exit(EXIT_FAILURE);
    }
    if(signal(SIGTERM, closing_handler) == SIG_ERR) {
        // TODO : Error management
        exit(EXIT_FAILURE);
    }
}

void closing_handler(int signal_number){
    close(server_fd);
    exit(EXIT_SUCCESS);
}

Message read_message(int fd){

}