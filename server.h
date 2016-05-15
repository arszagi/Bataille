//
// Created by Anthony Maton on 14/05/16.
//

#ifndef BATAILLE_SERVER_H
#define BATAILLE_SERVER_H

#include "messages.h"
void argument_check(int argc, char ** argv);
void register_signal_handlers();
void sig_end_handler(int signal_number);
Message read_message(int sd);
void set_lock();
int remove_lock();
int enough_players();
void start_game();

#endif //BATAILLE_SERVER_H
