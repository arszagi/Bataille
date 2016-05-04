//
// Created by Anthony Maton on 28/04/16.
//

#ifndef BATAILLE_SERVER_H
#define BATAILLE_SERVER_H

#include "messages.h"

int create_server(int port_number, int max_connections);
int connect_server(int port_number);
Message decode(char * string);

#endif //BATAILLE_SERVER_H
