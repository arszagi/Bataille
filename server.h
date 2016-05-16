//
// Created by Anthony Maton on 14/05/16.
//

#ifndef BATAILLE_SERVER_H
#define BATAILLE_SERVER_H

#include "messages.h"

/*
 * The registration timer between two registration if registrants >= 2
 * timer in seconds.
 */

#define REGISTRATION_TIMER 30

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

#endif //BATAILLE_SERVER_H
