/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */

#include <sys/socket.h>
#include "client.h"
#include "shared_memory.h"
#include "card.h"


Player my_player;
int server_socket;
Scoreboard *shared_memory_ptr;
struct reader_memory *reader_memory_ptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char ** argv){
    argument_check(argc, argv);
    ask_pseudo();

    /* On prépare les sockets */
    int port = atoi(argv[1]);
    server_socket = connect_server(port);
    send_message(REGISTER);
    Message validation;

    validation = read_message(server_socket);
    if (validation.type == INSCRIPTION_STATUS){
        if (validation.payload.number == TRUE) {
            printf("Inscription réussie. Veuillez patientiez, le jeu va bientôt commencer.\n");
        } else if (validation.payload.number == FALSE) {
            printf("Echec d'inscription.\n" );
            exit(EXIT_FAILURE);
        }
    }

    /* Initialisation de l'accès à la mémoire partagée */
    int shared_memory = create_shared_memory(FALSE);
    shared_memory_ptr = attach_memory(shared_memory);

    int reader_memory = create_shared_reader_memory(FALSE);
    reader_memory_ptr = access_shared_reader_memory(reader_memory);

    init_semaphore(FALSE);
    /* Fin initialisation mémoire partagée */

    while (TRUE) {
        Message message = read_message(server_socket);
        memcpy(my_player.hand,message.payload.hand,DECK_SIZE * sizeof(int));
        switch (message.type) {
            case END_GAME: {
                printf("Jeu terminé.");
                break;
            }
            case DISTRIBUTION_CARDS: {
                print_cards();
                send_card();
            }

            default:
                break;
        }
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
            printf("%s", name_is_too_short);
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

    }
    return val;
}

void print_cards(){
    int i;
    printf("Voici vos cartes: \n\t");
    for (i = 0; i < DECK_SIZE ; i++) {
        if (my_player.hand[i] == -1) {
            break;
        }
        printf("|%d -> ", i+1);
        print_card(my_player.hand[i]);
        printf("\t");
        if (i % 5 == 0 && i !=0 ) {
            printf("\n\t");
        }
    }
    printf("\n");
}

void send_card(){
    int card;
    printf("Veuillez choisir votre carte SVP\n");

}

