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
int number_cards;
int number_win_cards;
int last_round = FALSE;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char ** argv){
    argument_check(argc, argv);
    ask_pseudo();

    /* On prépare les sockets */
    int port = atoi(argv[1]);
    server_socket = connect_server(port);
    send_message(REGISTER, -1);
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
        switch (message.type) {
            case END_GAME: {
                end_game();
                break;
            }
            case DISTRIBUTION_CARDS: {
                last_round = FALSE;
                memcpy(my_player.hand,message.payload.hand,DECK_SIZE * sizeof(int));
                print_cards();
                send_card();
                break;
            }

            case RETURN_WIN_CARDS: {
                catch_win_cards(message);
                if (last_round == TRUE){
                    break;
                }
                print_cards();
                send_card();
                break;
            }
            case LOST_ROUND: {
                lost_round(message);
                if (last_round == TRUE){
                    break;
                }
                print_cards();
                send_card();
                break;
            }
            case END_ROUND: {
                last_round = TRUE;
                printf("End round\n");
                break;
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

void send_message(int code, int number){
    Message message;
    message.type = code;

    switch(code){
        case REGISTER: {
            snprintf(message.payload.name, sizeof(message.payload.name), "%s", my_player.name);
            send(server_socket, &message, sizeof(message), 0);
            break;
        }
        case SEND_CARD: {
            message.payload.number = number;
            send(server_socket, &message, sizeof(message), 0);
            break;
        }
        case LAST_CARD: {
            message.payload.number = number;
            send(server_socket, &message, sizeof(message), 0);
            break;
        }
        case SEND_SCORE: {
            message.payload.number = number;
            send(server_socket, &message, sizeof(message), 0);
            break;
        }
        case END_GAME: {

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
    number_cards = 0;
    printf("Voici vos cartes: \n\t");
    for (i = 0; i < DECK_SIZE ; i++) {
        if (my_player.hand[i] == -1) {
            break;
        }
        number_cards++;
        if (i < 9) {
            printf("| %d ", i + 1);
        } else {
            printf("|%d ", i + 1);
        }
        print_card(my_player.hand[i]);
        printf("\t");
        if (i % 5 == 0 && i !=0 ) {
            printf("\n\t");
        }
    }
    printf("\n");
}

/* envoie une carte au choix de la main */
void send_card(){
    int i, card = 0;
    char line[4];
    printf("Veuillez choisir le numéro qui corespond à votre carte SVP\n");
    while ( (fgets(line, 4, stdin )) != NULL ) {
        card = atoi(line);
        if (card == 0){
            printf("Sont autorisé que des chiffres, recommencez!");
        }
        if (card > number_cards) {
            printf("Vous avez choisis une carte qui existe pas dans le tas, recommencez!");
        } else {
            break;
        }
    }
    if (number_cards == 1){
        if (number_win_cards == 0) {
            send_message(LAST_CARD, my_player.hand[card-1]);
            remove_card(card-1);
        }else if (number_win_cards > 0){
            send_message(SEND_CARD, my_player.hand[card-1]);
            remove_card(card-1);
            for (i = 0; i < number_win_cards; i++){
                my_player.hand[i] = my_player.hand_win_cards[i];
            }
            number_win_cards = 0;
        }
    } else {
        send_message(SEND_CARD, my_player.hand[card-1]);
        remove_card(card-1);
    }
}

/* enleve la carte de la main*/
void remove_card(int card) {

    my_player.hand[card] = my_player.hand[number_cards-1];
    my_player.hand[number_cards-1] = NO_CARD;
    number_cards--;

}

void catch_win_cards(Message win_card) {
    int i;

    printf("\n*****************************************\n");
    printf("** Vous avez remporté cette bataille. ***\n");
    printf("*****************************************\n");
    printf("Vos cartes ganées: \t");
    for (i = 0; i < MAX_PLAYERS; i++ ) {
        if (win_card.payload.hand[i] == NO_CARD){
            break;
        }
        my_player.hand_win_cards[number_win_cards] = win_card.payload.hand[i];
        number_win_cards++;
        print_card(win_card.payload.hand[i]);
        printf("\t");
    }
    printf(LINE);
    if (last_round == TRUE) {
        send_score();
    }


}

void lost_round(Message lost_message) {
    int i;
    printf("\n************************************************\n");
    printf("*** Vous n'avez pas remporté cette bataille. ***\n");
    printf("************************************************\n");
    printf("Cartes gagnées par l'adversaire: \t");
    for (i = 0; i < MAX_PLAYERS; i++ ) {
        if (lost_message.payload.hand[i] == NO_CARD){
            break;
        }
        print_card(lost_message.payload.hand[i]);
        printf("\t");
    }
    printf(LINE);
    if (last_round == TRUE) {
        send_score();
    }
}


void send_score() {
    int score, i;
    score = compute_score(my_player.hand);
    score += compute_score(my_player.hand_win_cards);
    for (i = 0; i < DECK_SIZE; i++){
        my_player.hand[i] = NO_CARD;
        my_player.hand_win_cards[i] = NO_CARD;
    }
    send_message(SEND_SCORE, score);
}

void end_game(){

    semaphore_down(SEMAPHORE_ACCESS);
    if(shared_memory_ptr->players->name == my_player.name){
        printf("Votre score est: %d\n", shared_memory_ptr->players->score);
    }
    semaphore_up(SEMAPHORE_ACCESS);

    exit(EXIT_SUCCESS);

}

