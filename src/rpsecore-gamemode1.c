/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024 Wojciech Zduniak, Marcin Zduniak
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
#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-error.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define SERVER_USER_TYPE 1
#define CLIENT_USER_TYPE 2

/*
================
STATIC FUNCTIONS
================
*/

/*
char * must be freed by caller, see rpsecore-broadcast.h for P2P types. 
Username found in input_data->input.str_input 
*/
static unsigned short int
_rpse_gamemode1_getValidUsernameMenu(user_input_data_t *input_data, const unsigned short int USER_TYPE)
{
    bool exact_match_found = false;
    do
        {
        printf("Create a username for your player (30 character limit): ");

        input_data->buffer_size = 31;
        
        if (rpse_io_str(input_data, false) == EXIT_FAILURE)
            {
            perror("\"input_data->input.str_input == NULL\" while attempting to get stirng input");
            rpse_error_errorMessage("attempting to get string input");
            return EXIT_FAILURE;
            }

        string_dll_node_t *head = rpse_broadcast_receiveBroadcast();

        if (rpse_dll_deleteStringDLLDuplicateNodes(&head) == EXIT_SUCCESS)
            exact_match_found = rpse_broadcast_verifyAndTrimDLLStructure(&head, USER_TYPE, input_data->input.str_input);
        /* If there was nothing received here */
        else
            exact_match_found = false;

        if (exact_match_found)
            printf("This username has already been taken, please try again.\n");
        else
            printf("Great, your username will be %s!\n", input_data->input.str_input);
        
        if (head != NULL)
            rpse_dll_deleteStringDLL(&head);
    }
    while (exact_match_found);

    memset(input_data->input.str_input, 0, strlen(input_data->input.str_input) + 1);
    free(input_data->input.str_input);
    input_data->input.str_input = NULL;
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
        return EXIT_FAILURE;
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
    int user_type = _rpse_gamemode1_userTypeMenu(input_data);
    if (user_type == -1)
        {
        perror("Failure while getting user type");
        return EXIT_FAILURE;
        }
    _rpse_gamemode1_getValidUsernameMenu(input_data, user_type);
    
    /* BROADCAST USERNAME AFTER SUCCESSFUL MENU EXIT */

    return EXIT_SUCCESS;
}