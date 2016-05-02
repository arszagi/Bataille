//
// Created by Anthony Maton on 28/04/16.
//

#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include "network.h"
#include "constants.h"

int create_server(int port_number, int max_connections) {
    if(max_connections > MAX_PLAYERS || max_connections < 1){
        // TODO Add error management
        exit(EXIT_FAILURE);
    }
    int server_fd;
    int optval = 1;

    struct sockaddr_in socket_informations;

    /*
     * Première étape création du socket
     */
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        // TODO Error management
        perror("socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    /*
     * Deuxième étape binding du socket
     */
    bzero((char*)&socket_informations,sizeof(struct sockaddr_in));
    socket_informations.sin_family = AF_INET;
    socket_informations.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_informations.sin_port = htons(port_number);

    if(bind(server_fd, (struct sockaddr *) &socket_informations, sizeof(socket_informations)) < 0){
        // TODO Error management
        exit(EXIT_FAILURE);
    }

    /*
     * Troisième étape ecoute du serveur sur le port
     */
    if(listen(server_fd, 5) == -1 ){
        // TODO Error management
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

Message decode(char *string) {
    Message message;
    return message;
}
