//
// Created by Anthony Maton on 16/05/16.
//

#ifndef BATAILLE_LOGGER_H
#define BATAILLE_LOGGER_H

/*
 * Indique le niveau de l'entrée dans le log en respect de la norme POSIX (syslog)
 */

#define LOG_EMERGENCY 0
#define LOG_ALERT 1
#define LOG_CRITICAL 2
#define LOG_ERROR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

/*
 * Type de sortie du log
 */

#define WRITE_STDOUT 0
#define WRITE_FILED 1
#define LOG_OUTPUT_FOLDER "logs"

/*
 * Constante utilitaire
 */
#define ARRAY_SIZE 1024

#pragma clang diagnostic ignored "-Wvisibility"
#pragma clang diagnostic ignored "-Wformat"
int log_entry(char *message, int level);
int log_error(char *message, int level, int errno_number);
int set_log_output(int output_type);
char* format_entry(struct tm *time_info, int level, char *message, int errno_number);
char *alloc_log();

#endif //BATAILLE_LOGGER_H
