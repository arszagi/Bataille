//
// Created by Anthony Maton on 14/05/16.
//

#ifndef BATAILLE_SERVER_H
#define BATAILLE_SERVER_H

#include "messages.h"


/*
 * Status of our registration timer
 */

#define TIMER_OFF 0
#define TIMER_ON 1
#define TIMER_FINISHED 2

void argument_check(int argc, char ** argv);
void register_signal_handlers();
void alarm_timer_handler(int signal_number);
void sig_end_handler(int signal_number);
Message read_message(int sd);
void set_lock();
int remove_lock();
int enough_players();
void start_game();
void shared_memory_reset();
void send_cards();
void cancel_game(int sd);
void play_round();
void update_score(int socket, int score);
void update_scoreboard_final(Scoreboard * scoreboard);
int compute_score(int * deck);

#endif //BATAILLE_SERVER_H
