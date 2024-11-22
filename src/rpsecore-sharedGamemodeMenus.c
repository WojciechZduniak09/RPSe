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

#include "../include/rpsecore-sharedGamemodeMenus.h"
#include "../include/rpsecore-roundCalc.h"
#include "../include/rpsecore-moveDef.h"
#include "../include/rpsecore-broadcast.h"
#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-error.h"
#include "../include/rpsecore-io.h"
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <stdio.h>

/*
=======================
NON-MOVE-DEFINING MENUS
=======================

For move-defining menus, refer to rpsecore-moveDef.c
*/

void
rpse_sharedGamemodeMenus_roundSummary(round_info_t *round_info, move_data_t *move_data, player_data_t *player_data)
{
	if (strncmp(round_info->winner, "p1", 3) == 0)
        strcpy(round_info->winner, player_data->PLAYER_1_NAME);


    /* I know that this format is ugly but it shushes the compiler */
    else if (strncmp(round_info->winner, "p2", 3) == 0)
        {
        strcpy(round_info->winner, player_data->PLAYER_2_NAME);
        }
	
	printf("<----- ROUND RESULTS ----->\n");
	sleep(0.35);

	printf("Winner: %s.\n", round_info->winner);
	sleep(0.35);

    /* Player 1 data */

	printf("%s's info:\n", player_data->PLAYER_1_NAME);
	sleep(0.35);

	printf("\tSelected move: %s.\n", move_data->move_names[round_info->p1_move]);
	sleep(0.35);

	printf("\tCurrent win count: %u/3.\n", round_info->p1_wins);
	sleep(0.35);

    /* Player 2 data */

	printf("%s's info:\n", player_data->PLAYER_2_NAME);
	sleep(0.35);

	printf("\tSelected move: %s.\n", move_data->move_names[round_info->p2_move]);
	sleep(0.35);

	printf("\tCurrent win count: %u/3.\n\n", round_info->p2_wins);
	sleep(0.35);

    /* 15 second auto-close if there is no input */

    printf("\nThis menu will auto-close in 15 seconds.\n");

    time_t start_time = time(NULL);
    unsigned short int *ret_val;
    int status = -1;
    
    pthread_t thread;
    rpse_error_checkThreadCreation(thread);

    pthread_create(&thread, NULL, rpse_io_threadedEnterToContinue(), NULL);

    while (difftime(time(NULL), start_time) < 15)
        {
        status = pthread_join(thread, (void **)&ret_val);
        if (status == 0)
            break;
        }
        
    if (status != 0)
        {
        pthread_cancel(thread);
        printf("Auto-closing menu...");
        }
}

unsigned short int
rpse_sharedGamemodeMenus_endOfGameMenu(user_input_data_t *input_data, bool is_for_pve)
{
	const char PVE_MENU_OPTIONS[4][28] = {"Replay", "Edit custom move & replay", 
                                      "Return to main menu", "Exit RPSe"};
    const char PVP_MENU_OPTIONS[4][21] = {"Replay", "Return to PvP menu", "Return to main menu", 
                                          "Exit RPSe"};
    
    printf("<----- END OF GAME MENU ----->\n");

    for (unsigned short int option_index = 0; option_index < 4; option_index++)
        {
		sleep(0.5);
        if (is_for_pve)
		    printf("\t%u. %s.\n", option_index + 1, PVE_MENU_OPTIONS[option_index]);
        else
            printf("\t%u. %s.\n", option_index + 1, PVP_MENU_OPTIONS[option_index]);
	    }
	
    input_data->interval[0] = 1;
	input_data->interval[1] = 4;

	

    if (is_for_pve)
        rpse_io_int(input_data, false, "Choose an option by it's number: ");
    else
        {
        time_t start_time = time(NULL);
        input_data->input.int_input = -1;

        while (input_data->input.int_input == -1 && difftime(time(NULL), start_time) < 15)
            {
            rpse_io_int(input_data, false, "Choose an option by it's number: ");
            }
        }

	return input_data->input.int_input;
}

void
rpse_sharedGamemodeMenus_roundStartCountdown(void)
{
    printf("The game will start in 3");
    fflush(stdout);
    sleep(1);

    for (unsigned short int countdown_number = 2; countdown_number > 0; countdown_number--)
        {
        printf(", %u", countdown_number);
        fflush(stdout);
        sleep(1);
        }

    printf(", 0.\n");
    sleep(1);
}
