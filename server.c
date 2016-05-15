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
int main(int argc, char ** argv){

    int temp_sd, max_sd, i;
    int sd; // socket descriptor
    int select_result;

    argument_check(argc, argv);
    server_fd = create_server(atoi(argv[1]), MAX_PLAYERS);
    fd_set file_descriptor_set;

    /* Crée le fichier de lock qui empêche de lançer le logiciel plusieurs fois (entre autres) */
    set_lock();


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
            int socket_descriptor = client_socket[i];
            if(socket_descriptor > 0){
                FD_SET(socket_descriptor, &file_descriptor_set);
            }
            if(socket_descriptor > max_sd){
                socket_descriptor = max_sd;
            }
        }

        /* Attend une activité */
        select_result = select(max_sd + 1, &file_descriptor_set, NULL, NULL, NULL);

        if(select_result < 0){
            if(errno == EBADF){
                // TODO : Error management
            }else if(errno == EINTR){
                // TODO : Error management
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

            if(game_server.phase != REGISTRATION){
                // TODO : Manage the registration
            }
            if(message.type != REGISTER) {
                // TODO : Renvoyer une erreur
                continue;
            }

            /* On enregistre notre nouvel user */
            for(i = 0; i < MAX_PLAYERS; i++){
                if(game_server.players[i].socket != 0){
                    continue;
                }
                game_server.players[i].socket = temp_sd;
                fprintf(stderr, "Joueur : %s inscrit.", message.payload.name);
                Message ret;
                ret.type = INSCRIPTION_STATUS;
                ret.payload.number = 1;
                send(temp_sd, &ret, sizeof(ret), 0);
                // TODO : Ajouter une ligne de log
                break;
            }

            /* Le jeu a déjà commencé ou on a atteint MAX_PLAYERS */
            Message ret;
            ret.type = INSCRIPTION_STATUS;
            ret.payload.number = 0;
            send(temp_sd, &ret, sizeof(ret), 0);

            /* On ajoute ce nouveau socket à la table des sockets */
            for(i = 0; i < MAX_PLAYERS; i++){
                if(client_socket[i] == 0){
                    client_socket[i] = temp_sd;
                    break;
                }
            }

            User user = {};
            /* 8 is arbitrary defined and thus doesn't require a constant */
            strncpy(user.name, message.payload.name, 8);
            game_server.players[i].user = user;
            game_server.player_count += 1;

            if(timer_status == TIMER_OFF) {
                /* TODO : Réinitialiser la mémoire partagée */
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
                /* TODO : Lancer l'alarme avec un alarm ou un select ou je sais pas trop */
            }else if(timer_status == TIMER_FINISHED && enough_players()) {
                /* TODO : Ajouter une ligne de log */
                start_game();
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
    if( access(lock_file, F_OK)==0 ) {
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
    int i;
    int count = 0;
    for(i = 0; i < MAX_PLAYERS; i++){
        if(game_server.players[i].socket > 0) {
            count++;
        }
    }

    return count >= MIN_PLAYERS;
}

void start_game() {
    /* On change la phase de jeu */
    game_server.phase = PLAY;

    /* TODO : Mettre le reste de la magie */
}

void alarm_timer_handler(int signal_number) {
    /* TODO : Add a log entry */
    timer_status = TIMER_FINISHED;

    if(enough_players()) {
        start_game();
    }
}
