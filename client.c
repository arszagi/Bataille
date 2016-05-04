/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 * Point d'entrée de l'application client.
 */

#include <sys/socket.h>
#include "client.h"

Player my_player;
int server_socket;

int main(int argc, char ** argv){
    argument_check(argc, argv);

    fprintf(stdout, welcome, sizeof(welcome));
    ask_pseudo();

    /* On prépare les sockets */
    int port = atoi(argv[1]);
    server_socket = connect_server(port);

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

void ask_pseudo(){
    size_t length = 0;
    while(length < 1){
        fprintf(stdout, welcome, sizeof(welcome));
        if(fgets((char *) my_player.name, sizeof(my_player.name) + 1, stdin) == NULL){
            // TODO : Error management
            exit(EXIT_FAILURE);
        }

        length = strlen(strtok((char *) my_player.name, "\n"));
        if(length < 1){
            printf(name_is_too_short);
        }
    }
}

void send_message(int code){
    Message message;
    switch(code){
        case REGISTER: {
            message.type = REGISTER;
            snprintf(message.payload.name, sizeof(message.payload.name), "%s", my_player.name);
            send(server_socket, &message, sizeof(message), 0);
            break;
        }
        default: {
            break;
        }
    }
}