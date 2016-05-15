/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#include <sys/socket.h>
#include "client.h"
#include "shared_memory.h"


Player my_player;
int server_socket;
Scoreboard *shared_memory_ptr;
struct reader_memory *reader_memory_ptr;

int main(int argc, char ** argv){
    argument_check(argc, argv);

    fprintf(stdout, welcome, sizeof(welcome));
    ask_pseudo();

    /* On prépare les sockets */
    int port = atoi(argv[1]);
    server_socket = connect_server(port);
    send_message(REGISTER);
    Message validation;

    validation = read_message(server_socket);
    if (validation.type == INSCRIPTION_STATUS){
        if (validation.payload.number == TRUE) {
            fprintf(stdout, "Inscription réussie. Veuillez patientiez, le jeu va bientôt commancer.");
        } else if (validation.payload.number == FALSE) {
            fprintf(stdout, "Echec d'inscription." );
        }
    }

    /* Initialisation de l'accès à la mémoire partagée */
    int shared_memory = create_shared_memory(FALSE);
    shared_memory_ptr = attach_memory(shared_memory);

    int reader_memory = create_shared_reader_memory(FALSE);
    reader_memory_ptr = access_shared_reader_memory(reader_memory);

    init_semaphore(FALSE);
    /* Fin initialisation mémoire partagée */


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

Message read_message(int sd){
    Message val;
    if(recv(sd, &val, sizeof(val), 0) <= 0){
        /* TODO : Erreur ou déconnexion ??? Je sais pas */
    } else {

    }
    return val;
}

