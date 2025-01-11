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

#include "../include/rpsecore-gamemode1.h"
#include "../include/rpsecore-io.h"
#include "../include/rpsecore-broadcast.h"
#include "../include/rpsecore-discovery.h"
#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-error.h"
#include "../include/rpsecore-moveDef.h"
#include <regex.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

pthread_t receiver_loop_thread_ID, broadcaster_loop_thread_ID;
/*
================
STATIC FUNCTIONS
================
*/

/* ip must be freed */
static char *
_rpse_gamemode1_getIPAddress(void)
{
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1)
        {
        perror("getifaddrs");
        rpse_error_errorMessage("attempting to get ifaddrs");
        return NULL;
        }
    
    struct ifaddrs *ifa;
    int family;
    char *host = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && host == NULL; attempt++)
        host = calloc(NI_MAXHOST, sizeof(char));
    if (host == NULL)
        {
        perror("calloc");
        return NULL;
        }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET)
            {
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST))
                {
                perror("getnameinfo");
                return NULL;
                }
            }
        }
    
    freeifaddrs(ifaddr);
    return host;
}

/* Username stores in *input_data->input.str_input*. See rpsecore-broadcast.h for user types. */
static unsigned short int
_rpse_gamemode1_getValidUsernameMenu(user_input_data_t *input_data, const unsigned short int USER_TYPE)
{
    bool exact_match_found = false;
    bool username_is_valid = false;
    input_data->buffer_size = 31;
    do
        {
	while (!username_is_valid)
            {
            printf("Create a username for your player (30 character limit, no @ symbols allowed): ");

                if (rpse_io_str(input_data, false) == EXIT_FAILURE)
            	    {
            	    perror("\"input_data->input.str_input == NULL\" while attempting to get stirng input");
                    rpse_error_errorMessage("attempting to get string input");
                    return EXIT_FAILURE;
                    }
    	    unsigned int username_len = strlen(input_data->input.str_input) + 1;
	    unsigned int current_character_index = 0;
	    int current_character;
	    do
	        {
                current_character = input_data->input.str_input[current_character_index];
	        if (current_character == '@')
	            {
		    printf("Invalid input! No \"@\" symbols are allowed!\n");
		    username_is_valid = false;
		    break;
		    }
		else
		    username_is_valid = true;
		current_character_index++;
		}
	    while (current_character_index < username_len);
	    }
	printf("Please wait...\n");
        broadcast_data_t tmp_broadcast_data;
	tmp_broadcast_data.user_type = USER_TYPE;
	strncpy(tmp_broadcast_data.username, input_data->input.str_input, strlen(input_data->input.str_input) + 1);
	rpse_broadcast_waitUntilInterval();
        string_dll_node_t *head = rpse_broadcast_receiveBroadcast(&tmp_broadcast_data);

	regex_t verification_regex;
	char pattern[36] = "";
	strncat(pattern, input_data->input.str_input, 31);
	strncat(pattern, "@RPSe", 6);
	int ret_val = regcomp(&verification_regex, (const char *)&pattern, REG_EXTENDED);
	if (ret_val)
		{
		perror("Unable to compile regex.");
		rpse_error_errorMessage("attempting to compile a regular expression");
		return EXIT_FAILURE;
		}
	
	if (head == NULL)
		exact_match_found = false;
	else
		{
		string_dll_node_t *current_node = head;
		while (current_node == NULL)
			{
			ret_val = regexec(&verification_regex, current_node->data, 0, NULL, 0);
			if (!ret_val)
				{
				exact_match_found = true;
				current_node = NULL;
				continue;
				}
			else if (ret_val != REG_NOMATCH)
				{
				/* In case of failure */
				perror("Unable to execute a regex");
				rpse_error_errorMessage("attempting to regexec()");
				return EXIT_FAILURE;
				}
			if (current_node->next == NULL)
				current_node = NULL;
			else
				current_node = current_node->next;
			}
		}
	regfree(&verification_regex);

        if (exact_match_found)
            printf("This username has already been taken, please try again.\n");
        else
            printf("\nGreat, your username will be %s!\n", input_data->input.str_input);
        
        if (head != NULL)
            rpse_dll_deleteStringDLL(&head);
    }
    while (exact_match_found);

    return EXIT_SUCCESS;
}


static short int
_rpse_gamemode1_userTypeMenu(user_input_data_t *input_data)
{
    if (input_data == NULL)
	{
	perror("\"input_data == NULL\" while attempting to display player menu");
	return -1;
	}
    
    printf("<---- Player menu ---->\n");

    input_data->interval[0] = 1;
    input_data->interval[1] = 2;

    printf("Would you like to host or join a game?\n");
    sleep(0.25);
    printf("1. Host.\n"
           "2. Join.\n\n");
    
    if (rpse_io_int(input_data, false, "Select a role by it's number: ") == EXIT_FAILURE)
        {
        perror("Failure while attempting to get int input");
        return 3; /* 3 is the failure code here, i cant be bothered to explain why lol */
        }
    
    if (input_data->input.int_input == 1)
        return SERVER_USER_TYPE;
    else
        return CLIENT_USER_TYPE;
}

/*
============
PVP FUNCTION
============
*/

unsigned short int
rpse_gamemode1_pvp(user_input_data_t *input_data)
{
    printf("This gamemode is still a work in progress, please try again in a future update.\n");
    printf("Here is a sneak peek:\n\n");
    sleep(1);
    
    broadcast_data_t broadcast_data;
    broadcast_data.user_type = _rpse_gamemode1_userTypeMenu(input_data);
    if (broadcast_data.user_type == 3)
        {
        perror("Failure while getting user type");
        return EXIT_FAILURE;
        }

    _rpse_gamemode1_getValidUsernameMenu(input_data, broadcast_data.user_type);
    strncpy(broadcast_data.username, input_data->input.str_input, strlen(input_data->input.str_input) + 1);
    
    memset(input_data->input.str_input, 0, strlen(input_data->input.str_input) + 1);
    free(input_data->input.str_input);
    input_data->input.str_input = NULL;

    /* Creating the message and setting up the custom move here */
    snprintf(broadcast_data.message, input_data->buffer_size, "%s", broadcast_data.username);
    strcat(broadcast_data.message, "@RPSe.");

    char *IP_address = _rpse_gamemode1_getIPAddress();

    char port[6];
    snprintf(port, sizeof(port), "%d", BROADCASTER_PORT);

    move_data_t *move_data;
    if (broadcast_data.user_type == SERVER_USER_TYPE)
    	{
	    move_data = rpse_moveDef_setUpMoves(input_data);
	    strcat(broadcast_data.message, "server/bindOn(");
    	    strcat(broadcast_data.message, IP_address);
	    strcat(broadcast_data.message, ")(");
	    strcat(broadcast_data.message, port);
	    strcat(broadcast_data.message, ")/customMove(");
	    strcat(broadcast_data.message, move_data->move_names[3]);
	    strcat(broadcast_data.message, ")(");
	    for (unsigned short int index = 0; index < 3; index++)
	    	{
		/* This checks the 2D array's second element (the loser */
		if (move_data->winning_combinations[index + 3][1] == 3)
			strcat(broadcast_data.message, "f"); /* f for false, does not beat (0=rock, 1=paper, 2=scissors */
		else
			strcat(broadcast_data.message, "t"); /* t for true */
		}
	    strcat(broadcast_data.message, ")");
	    }
    else
    	{
	    move_data = NULL;
	    strcat(broadcast_data.message, "client/invitesOn(");
    	    strcat(broadcast_data.message, IP_address);
	    strcat(broadcast_data.message, ")(");
	    strcat(broadcast_data.message, port);
	    strcat(broadcast_data.message, ")");
	    }

    free(IP_address);
    IP_address = NULL;

    printf("\nRPSe will now start searching for players on your network, this shouldn't take over 20 seconds.\n");
    printf("If you wish to stop searching for players at any time, please press Ctrl+C.\n\n");

    int ret_val = pthread_create(&broadcaster_loop_thread_ID, NULL, (void *)rpse_discovery_broadcasterLoop, (broadcast_data_t *)&broadcast_data);
    if (ret_val != EXIT_SUCCESS)
	{
	perror("\"ret_val != EXIT_SUCCESS\" while trying to start broadcaster loop\n");
	rpse_error_errorMessage("attempting to start a thread");
	abort();
	}
    ret_val = pthread_create(&receiver_loop_thread_ID, NULL, (void *)rpse_discovery_receiverLoop, (broadcast_data_t *)&broadcast_data);
    if (ret_val != EXIT_SUCCESS)
        {
        perror("\"ret_val != EXIT_SUCCESS\" while trying to start receiver loop\n");
        rpse_error_errorMessage("attempting to start a thread");
        abort();
        }

    pthread_join(broadcaster_loop_thread_ID, NULL);
    pthread_join(receiver_loop_thread_ID, NULL);
    
    if (broadcast_data.user_type == SERVER_USER_TYPE) /* Defined in *rpsecore-broadcast.h* */
        rpse_moveDef_freeMoveData(move_data);
    move_data = NULL;

    return EXIT_SUCCESS;
}
