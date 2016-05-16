//
// Created by Anthony Maton on 16/05/16.
//

#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "constants.h"

/*
 * Sortie utilisé pour les messages de log
 * STDOUT par défaut.
 */
int log_output = WRITE_STDOUT;

/*
 * Permet d'ajouter une entrée dans le log en ajoutant le errno (voir errno.h)
 * vaut -1 si il n'y a pas d'erreur
 */
int log_error(char *message, int level, int errno) {
    int output_descriptor = 1;
    char *log_entry;
    char filepath[ARRAY_SIZE];
    char *home_directory;

    /* Récupération du temps système (pour permettre une meilleure lecture du log) */
    time_t timestamp = time(NULL);
    struct tm *time = gmtime(&timestamp);

    /* Si il s'agit d'une sortie dans un fichier, on réalise une série d'instruction préparatoires */
    home_directory = getpwuid(getuid())->pw_dir;
    sprintf(filepath, "%s/%s", home_directory, LOG_OUTPUT_FOLDER);
    /* Crée un repertoire avec les permissions RWX pour User et Group et R pour Other */
    mkdir(filepath, S_IRWXU | S_IRWXG | S_IROTH);

    /* Crée le fichier de log basé sur la date et l'heure */
    sprintf(filepath, "%s/%s/bataille_%d_%d.log", home_directory, LOG_OUTPUT_FOLDER,
            time->tm_mday + 1, time->tm_mon + 1);

    if((output_descriptor = open(filepath, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IROTH)) == ERROR) {
        log_error("Unable to open or create the log file", LOG_WARNING, errno);
        return ERROR;
    }

    log_entry = format_entry(time, level, message, errno);

    /* On écrit l'entrée dans le fichier de log */
    if(write(output_descriptor, log_entry,  strlen(log_entry)) == ERROR){
        log_error("Unable to write in the log file", LOG_WARNING, errno);
    }

    /* On libère le pointeur de message et on ferme le fichier si on emploie un fichier */
    free(log_entry);
    if(log_output == WRITE_FILED) {
        close(output_descriptor);
    }

    return 0;
}

/*
 * Permet d'ajouter une entrée dans le log
 */
int log_entry(char *message, int level) {
    return 0;
}

/*
 * Défini la sortie du log renvoie 0 en cas de succès
 * renvoie -1 en cas d'erreur
 */
int set_log_output(int output_type) {
    if(output_type == WRITE_STDOUT) {
        log_output = WRITE_STDOUT;
        return 0;
    } else if(output_type == WRITE_FILED) {
        log_output = WRITE_FILED;
        return 0;
    }
    return ERROR;
}

/*
 * Formatte le message pour être inseré dans notre log.
 * L'application étant client serveur, il nous a semblé intéressant d'imaginer
 * que les logs pourraient être stocké sur un serveur ailleurs et donc de joindre
 * la timezone plutôt que de la calculer.
 * TODO : Discuter avec Kamil de si c'est justifié ou non
 */
char* format_entry(struct tm *time, int level, char *message, int errno) {
    char *entry;

    if((entry = (char *) malloc(ARRAY_SIZE * sizeof(char))) == NULL) {
        log_error("An error occured during malloc", LOG_ALERT, errno);
    }

    if(errno < 0) {
        /* Message de log standard sans errno à la fin */
        sprintf(entry, "%d -- %d %d/%d %d:%d:%d : \t %s \n",
                level,
                time->tm_mday + 1,
                time->tm_mon + 1,
                time->tm_hour,
                time->tm_min,
                time->tm_sec,
                message
        );
    }else {
        /* Message de log avec errno */
        sprintf(entry, "%d -- %d %d/%d %d:%d:%d : \t %s : \t %s \n",
                level,
                time->tm_mday + 1,
                time->tm_mon + 1,
                time->tm_hour,
                time->tm_min,
                time->tm_sec,
                message,
                strerror(errno)
        );
    }

    return entry;
}
