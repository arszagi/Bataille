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
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <sys/fcntl.h>
#include "utils.h"
#include "constants.h"
#include "network.h"
#include "game.h"
#include "shared_memory.h"
#include "server.h"


int server_fd;
Game game_server;
int timer_status = TIMER_OFF;

/* Mémoire partagée */
int shared_memory;
Scoreboard *shared_memory_ptr;
int reader_memory;
struct reader_memory *reader_memory_ptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wunused-value"
int main(int argc, char ** argv){

    int temp_sd, max_sd, i;
    int sd; // socket descriptor
    int select_result;

    argument_check(argc, argv);
    server_fd = create_server(atoi(argv[1]), MAX_PLAYERS);
    fd_set file_descriptor_set;

    /* Crée le fichier de lock qui empêche de lançer le logiciel plusieurs fois (entre autres) */
    set_lock();

    /* Active la gestion de signal */
    register_signal_handlers();

    /* Nous avons un socket par joueur, il nous faut donc MAX_PLAYERS sockets d'ouverts ! */
    int client_socket[MAX_PLAYERS];


    for (i = 0; i < MAX_PLAYERS; i++) {
        client_socket[i] = 0;
    }

    /* Initialisation de la mémoire partagée (Scoreboard) et des sémaphores */
    shared_memory = create_shared_memory(TRUE);
    shared_memory_ptr = attach_memory(shared_memory);
    init_semaphore(TRUE);
        /* TODO : Il faudrait un peu plus d'explications pour être sûr qu'on comprenne bien */
    reader_memory = create_shared_memory(TRUE);
    reader_memory_ptr = access_shared_reader_memory(reader_memory);
    reader_memory_ptr->reader_count = 0;

    /*
     * Initialisation du scoreboard d'un round
     */
    Scoreboard round_board;

    /*
     * Le serveur ne s'arrête jamais, nous avons donc recours à une boucle infinie
     * while(TRUE) et à des conditions de ruptures en cas d'erreur
     */

    while(TRUE){

        // Ici notre serveur doit tourner.
        FD_ZERO(&file_descriptor_set);
        FD_SET(server_fd, &file_descriptor_set);
        max_sd = server_fd;

        /* Configure l'écoute sur nos 4 sockets */
        for(i = 0; i < MAX_PLAYERS; i++){
            sd = client_socket[i];
            if(sd > 0){
                FD_SET(sd, &file_descriptor_set);
            }
            if(sd > max_sd){
                max_sd = sd;
            }
        }

        /* Attend une activité */
        select_result = select(max_sd + 1, &file_descriptor_set, NULL, NULL, NULL);

        if(select_result < 0){
            if(errno == EBADF){
                printf("File descriptor removed!\n");
            }else if(errno == EINTR){
                printf("Interrupted system call.\n");
            }
            continue;
        }

        /* Nouvelle connexion */
        if(FD_ISSET(server_fd, &file_descriptor_set)){

            if((temp_sd = accept(server_fd, NULL, 0)) < 0) {
                // TODO : Error management
                raise(SIGTERM);
            }

            Message message = read_message(temp_sd);

            /* Annuler si le jeu a deja commence*/
            if(game_server.phase != REGISTRATION){
                cancel_game(temp_sd);
                continue;
            }

            /* On enregistre notre nouvel user */
            if(message.type == REGISTER) {
                for(i = 0; i < MAX_PLAYERS; i++){
                    if(game_server.players[i].socket != 0){
                        continue;
                    }
                    game_server.players[i].socket = temp_sd;
                    printf("Joueur : %s inscrit. socket: %d \n", message.payload.name, temp_sd);
                    Message ret;
                    ret.type = INSCRIPTION_STATUS;
                    ret.payload.number = 1;
                    send(temp_sd, &ret, sizeof(ret), 0);
                    // TODO : Ajouter une ligne de log
                    break;
                }
            }

            /* Si on a atteint MAX_PLAYERS */
            cancel_game(temp_sd);

            /* On ajoute ce nouveau socket à la table des sockets */
            for(i = 0; i < MAX_PLAYERS; i++){
                if(client_socket[i] == 0){
                    client_socket[i] = temp_sd;
                    break;
                }
            }

            User user = {};
            strncpy(user.name, message.payload.name, NAME_SIZE);
            user.socket = temp_sd;
            game_server.players[i].user = user;
            game_server.player_count += 1;

            /*
             * Si le timer est éteind, il s'agit d'une nouvelle partie :
             * On réinitialise la mémoire partagée.
             */
            if(timer_status == TIMER_OFF) {
                shared_memory_reset();
            }

            semaphore_down(SEMAPHORE_ACCESS);
            shared_memory_ptr->players[i] = user;
            semaphore_up(SEMAPHORE_ACCESS);

            /* TODO : Ajouter une ligne de log signalant l'inscription du nouveau joueur */

            /*
             * Si le timer est OFF (Il s'agit du premier inscrit, on lance le timer
             * Si le timer est fini et que nous avons deux joueurs ou plus, on lance le jeu
             */
            if(timer_status == TIMER_OFF) {
                /* TODO : Ajouter une ligne de log. */
                alarm(WAITING_TIME);
                timer_status = TIMER_ON;
            }else if(timer_status == TIMER_FINISHED && enough_players()) {
                /* TODO : Ajouter une ligne de log */
                start_game();
            }
        }
        /*
         * Il s'agit d'un client déjà enregistré
         */
        else {
            for(i = 0; i < MAX_PLAYERS; i++) {
                temp_sd = game_server.players[i].socket;
                if(temp_sd == 0 || !FD_ISSET(temp_sd, &file_descriptor_set)) {
                    continue;
                }

                Message message = read_message(temp_sd);

                switch(message.type) {
                    case DISCONNECT: {
                        for(i = 0; i < game_server.player_count; i++) {
                            if(game_server.players[i].socket == temp_sd){
                                game_server.players[i] = game_server.players[game_server.player_count - 1];
                                game_server.player_count--;
                                break;
                            }
                        }
                        if(!enough_players()) {
                            Message winner;
                            winner.type = IS_WINNER;
                            send(game_server.players[0].socket, &winner, sizeof(Message), 0);
                            exit(EXIT_SUCCESS);
                        }
                        /* TODO : Ajouter une ligne de log */
                        printf("Déconnexion avec succes du jouer au socket : %d \n", temp_sd);
                        break;
                    }
                    case SEND_CARD: {
                        printf("Carte recu (%d) du socket: %d\n", message.payload.number, temp_sd);
                        if (game_server.phase != PLAY) {
                            break;
                        }
                        game_server.working_memory[i] = message.payload.number;
                        game_server.played += 1;
                        /* On vérifie que tout les joueurs ait joué
                         * le dernier joueur triggerera le calcul du gagnant et du score
                         */
                        if (game_server.played < game_server.player_count) {
                            continue;
                        } else {
                            play_round();
                            game_server.played = 0;
                            int reset_walker;
                            for (reset_walker = 0; reset_walker < game_server.player_count; reset_walker++) {
                                game_server.working_memory[reset_walker] = -1;
                            }
                        }
                        break;
                    }
                    case LAST_CARD: {
                        Message end_round;
                        end_round.type = END_ROUND;
                        for (i = 0; i < game_server.player_count; i++) {
                            send(game_server.players[i].socket, &end_round, sizeof(Message), 0);
                        }
                        /* TODO : Ajouter une ligne de log : Fin de manche */
                        break;
                    }
                    case SEND_SCORE: {
                        update_score(temp_sd, message.payload.number);
                        break;
                    }

                }
            }
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

void register_signal_handlers() {
    // End signal (CTRL+C)
    if (signal(SIGINT, sig_end_handler) == SIG_ERR) {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    // Kill signal
    if (signal(SIGTERM, sig_end_handler) == SIG_ERR ) {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    // Timer alarm
    if (signal(SIGALRM, alarm_timer_handler) == SIG_ERR) {
        int i;
        Message cancel;
        cancel.type = CANCEL;
        for(i = 0; i < game_server.player_count; i++) {
            send(game_server.players[i].socket, &cancel, sizeof(Message), 0);
        }
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }
}

/*
 * Appeler en cas de signal de fin
 * Libère les ressources IPC, Sémaphores, Lock, Socket, ...
 */
void sig_end_handler(int signal_number){
    shmdt(shared_memory_ptr);
    shmctl(shared_memory, IPC_RMID, NULL);

    shmdt(reader_memory_ptr);
    shmctl(reader_memory, IPC_RMID, NULL);

    /* TODO : Signaler que l'effacement de la shared memory est un succès */

    delete_semaphores();
    remove_lock();
    close(server_fd);
    exit(EXIT_SUCCESS);
}

Message read_message(int sd){
    Message message;
    if(recv(sd, &message, sizeof(message), 0) <= 0){
        /* TODO : Erreur ou déconnexion ??? Je sais pas */
    } else {

    }
    return message;
}

void set_lock(){
    char lock_file[MAX_ARRAY_SIZE];

    /* Fichier à bloquer */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    sprintf(lock_file,"%s/bataille.lock", home_dir);

    /* Vérifie que le fichier est accessible */
    if( access(lock_file, F_OK) == 0 ) {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }

    if ((open(lock_file, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH)) == -1) {
        /* TODO : Error management */
        exit(EXIT_FAILURE);
    }
    /* TODO : Error management */
}

int remove_lock() {
    char lock_file[MAX_ARRAY_SIZE];

    /* Fichier lock */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    sprintf(lock_file,"%s/bataille.lock", home_dir);

    if( unlink(lock_file)==-1 ) {
        /* TODO : Error management */
        return 1;
    }

    /* TODO : Error management */
    return 0;
}

/*
 * Vérifie que nous avons au moins deux joueurs
 */
int enough_players() {
    return game_server.player_count >= MIN_PLAYERS;
}

void start_game() {
    /* On change la phase de jeu */
    game_server.phase = PLAY;

    send_cards();

    /* TODO : Mettre le reste de la magie */
}

void alarm_timer_handler(int signal_number) {
    /* TODO : Add a log entry */
    timer_status = TIMER_FINISHED;

    if(enough_players()) {
        start_game();
    }
}

void shared_memory_reset() {
    int i;
    User user = {};
    semaphore_down(SEMAPHORE_ACCESS);
    for (i = 0; i < MAX_PLAYERS; i++) {
        shared_memory_ptr->players[i] = user;
    }
    semaphore_up(SEMAPHORE_ACCESS);
}

void send_cards(){
    int * deck = fill_deck();
    int dist_card = DECK_SIZE / game_server.player_count;
    int i, j, ptr = 0;
    Message hand;

    for (i = 0; i < game_server.player_count ; i++ ) {
        for (j = 0; j < dist_card; j++) {
            game_server.players[i].hand[j] = deck[ptr];
            ptr++;
        }
        for (j; j < DECK_SIZE; j++) {
            game_server.players[i].hand[j] = NO_CARD;
        }
        hand.type = DISTRIBUTION_CARDS;
        //hand.payload.hand = game_server.players[i].hand;
        memcpy(hand.payload.hand, game_server.players[i].hand, DECK_SIZE * sizeof(int));
        if (send(game_server.players[i].socket, &hand, sizeof(hand),0) <= 0) {
            // TODO : log
        }
    }

}
/* Annulation de la partie */
void cancel_game(int sd) {
    Message ret;
    ret.type = INSCRIPTION_STATUS;
    ret.payload.number = FALSE;
    send(sd, &ret, sizeof(ret), 0);
}

void play_round() {
    int i;
    int pool[MAX_PLAYERS];
    for (i = 0; i < MAX_PLAYERS; i++) {
        pool[i] = NO_CARD;
    }
    int winner_socket;
    for(i = 0; i < game_server.player_count - 1; i++){
        if(game_server.working_memory[i] > game_server.working_memory[i+1]) {
            winner_socket = game_server.players[i].socket;
        }else {
            winner_socket = game_server.players[i + 1].socket;
        }
        /* TODO : Optimize */
        pool[i] = game_server.working_memory[i];
        pool[i + 1] = game_server.working_memory[i+1];
    }

    /* Ce qu'on envoie au gagnant de la manche */
    Message winner;
    winner.type = 5;
    memcpy(winner.payload.hand, pool, MAX_PLAYERS * sizeof(int));


    /* display */
    printf("%d %d %d \n",winner_socket, winner.payload.hand[0], winner.payload.hand[1]);
    /* end of display */

    send(winner_socket, &winner, sizeof(Message), 0);


    /* Ce qu'on envoie aux perdants de la manche */
    Message looser = winner;
    looser.type = 10;

    for(i = 0; i < game_server.player_count; i++){
        if(game_server.players[i].socket == winner_socket) {
            continue;
        }
        send(game_server.players[i].socket, &looser, sizeof(Message), 0);
    }

}

void update_score(int socket, int score) {
    semaphore_down(SEMAPHORE_ACCESS);
    int i;
    for(i = 0; i < game_server.player_count; i++){
        if(shared_memory_ptr->players[i].socket == socket){
            shared_memory_ptr->players[i].score += score;
            break;
        }
    }
    semaphore_up(SEMAPHORE_ACCESS);

}
