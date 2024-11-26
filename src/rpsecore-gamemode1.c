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
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/*
================
STATIC FUNCTIONS
================
*/

/*
char * must be freed by caller, see rpsecore-broadcast.h for P2P types. 
Username found in input_data->input.str_input 
*/
void
_rpse_gamemode1_usernameMenu(user_input_data_t *input_data, const unsigned short int P2P_TYPE)
{
    bool exact_match_found = false;
    do
        {
        printf("Insert a username for your player: ");
        
        rpse_io_str(input_data, false);

        dll_node_t *head = rpse_broadcast_receiveBroadcast();
        rpse_dll_deleteDLLDuplicateNodes(&head);
        exact_match_found = rpse_broadcast_verifyDLLStructure(&head, P2P_TYPE, input_data->input.str_input);

        if (exact_match_found)
            printf("This username has already been taken, please try again.\n");
        
        rpse_dll_deleteDLL(&head);
    }
    while (exact_match_found);

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
    sleep(1);

    printf("Here is a sneek peak:\n\n");
    sleep(1);
    _rpse_gamemode1_usernameMenu(input_data, 1);
    /* BROADCAST USERNAME AFTER SUCCESSFUL MENU EXIT */

    return 0;
}