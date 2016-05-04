/*
 * Série 3
 * Arszagi vel Harszagi Kamil : karszag15
 * Maton Anthony : amaton15
 *
 */
#ifndef BATAILLE_SERVER_H
#define BATAILLE_SERVER_H

#include "messages.h"

int create_server(int port_number, int max_connections);
int connect_server(int port_number);
Message decode(char * string);

#endif //BATAILLE_SERVER_H
