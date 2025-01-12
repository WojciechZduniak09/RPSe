/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
 *
 * This file is part of RPSe.
 *
 * RPSe is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with RPSe.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-broadcast.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

/*
================
GLOBAL VARIABLES
================
*/

pthread_mutex_t termination_lock = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t broadcaster_termination_flag = 0;
volatile sig_atomic_t receiver_termination_flag = 0;

/*
================
STATIC FUNCTIONS
================
*/

static void 
_rpse_discovery_handleInternalTerminationSignal(const int SIGNAL)
{
    if (SIGNAL == SIGUSR1 || SIGNAL == SIGUSR2)
	pthread_mutex_lock(&termination_lock);
    else
	return;

    if (SIGNAL == SIGUSR1)
        broadcaster_termination_flag = 1;
    else if (SIGNAL == SIGUSR2)
        receiver_termination_flag = 1;

    pthread_mutex_unlock(&termination_lock);
}

static void
_rpse_discovery_handleUserTerminationSignal(const int SIGNAL)
{
	if (SIGNAL == SIGINT)
		{
		printf("\nProcess aborted!\n");
		_rpse_discovery_handleInternalTerminationSignal(SIGUSR1);
		_rpse_discovery_handleInternalTerminationSignal(SIGUSR2);
		}
}

/*
===================================
BROADCASTER/RECEIVER LOOP FUNCTIONS
===================================
*/

/* Should be threaded, view broadcast header for user types */
void *
rpse_discovery_broadcasterLoop(broadcast_data_t *broadcast_data)
{
    if (broadcast_data == NULL)
	{
	perror("rpse_discovery_broadcasterLoop() --> broadcast_data == NULL");
	pthread_exit(NULL);
	return NULL;
	}

    signal(SIGUSR1, _rpse_discovery_handleInternalTerminationSignal);
    signal(SIGINT, _rpse_discovery_handleUserTerminationSignal);

    while (broadcaster_termination_flag == 0)
        {
        rpse_broadcast_waitUntilInterval();
        rpse_broadcast_publishBroadcast(broadcast_data);
	sleep(10);
        }

    signal(SIGUSR1, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    pthread_exit(NULL);
    return NULL;
}

/* Should be threaded, view broadcast header for user types */
void *
rpse_discovery_receiverLoop(const broadcast_data_t *BROADCAST_DATA)
{
    if (BROADCAST_DATA == NULL)
        {
	perror("rpse_discovery_receiverLoop() --> BROADCAST_DATA == NULL");
	pthread_exit(NULL);
        return NULL;
	}
    else if (BROADCAST_DATA->user_type != SERVER_USER_TYPE && BROADCAST_DATA->user_type != CLIENT_USER_TYPE)
	{
	perror("rpse_discovery_receiverLoop() --> BROADCAST_DATA == NULL");
	pthread_exit(NULL);
	return NULL;
	}
    
    signal(SIGUSR2, _rpse_discovery_handleInternalTerminationSignal);
    signal(SIGINT, _rpse_discovery_handleUserTerminationSignal);

    unsigned int attempt = 0;
    while (receiver_termination_flag == 0)
        {
	printf("\n<----- Attempt %u ----->\n", attempt + 1);
        
	rpse_broadcast_waitUntilInterval();
        string_dll_node_t *head = rpse_broadcast_receiveBroadcast(BROADCAST_DATA);
	sleep(2);

	printf("List of players found on your network:\n\n");
	if (head == NULL || head->data == NULL)
		{
		printf("No players found on your WiFi network.\n\n");
		printf("Suggestions:\n"
		       "1. Wait a bit longer.\n"
		       "2. Change networks and restart RPSe.\n"
		       "3. Cancel by pressing Ctrl+C.\n"
		       "Note: cancelling happens after the end of an attempt, this is expected, please be patient if so.\n\n");
		attempt++;
		continue;
		}
	else if (head->next == NULL)
        	printf("1. %s\n", head->data);
	else
		{
		string_dll_node_t *current_node = head;
		unsigned int player_number = 1;
		while (current_node != NULL)
			{/*
			if (user_type == SERVER_USER_TYPE)
				printf("%d) %s\n", player_number , USERNAME);
			else  If it's a client 
				{
				printf("%d) %s\n"
				      "\tCustom move \"%s\" beats:\n", player_number, USERNAME, MOVE4);
				 GET AND PRINTF MOVE DATA HERE 
				}*/
			printf("%d) %s\n", player_number, current_node->data);
			if (current_node->next == NULL)
				current_node = NULL;
			}
		}
	rpse_dll_deleteStringDLL(&head);
	attempt++;
        }

    signal(SIGUSR2, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    pthread_exit(NULL);
    return NULL;
}
