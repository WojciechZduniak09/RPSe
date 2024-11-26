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

#include "../include/rpsecore-gamemode2.h"
#include "../include/rpsecore-io.h"
#include "../include/rpsecore-sharedGamemodeMenus.h"
#include "../include/rpsecore-roundCalc.h"
#include "../include/rpsecore-moveDef.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/*
============
PVE FUNCTION
============
*/

unsigned short int rpse_gamemode2_pve(user_input_data_t *input_data) {
    printf("\nWelcome to PvE! Before playing, you'll have to set up a few things...\n");

    move_data_t *move_data = rpse_moveDef_setUpMoves(input_data);

    rpse_sharedGamemodeMenus_roundStartCountdown();

    round_info_t round_info =
    {
        .winner = "n/a",
        .p1_wins = 0,
        .p1_move = 0,
        .p2_wins = 0,
        .p2_move = 0,
        .replay = true,
        .round_num = 1
    };

    while (round_info.replay == true && (round_info.p1_wins < 3 || round_info.p2_wins < 3))
        {
        printf("\n");
        printf("<----- ROUND %u ----->\n\n", round_info.round_num);
        sleep(0.5);

        printf("<---- PLAYER'S TURN ---->\n\n");
        sleep(0.5);

        printf("<--- Move options --->\n");

        for (unsigned short int move_index = 0; move_index < 4; move_index++)
            {
            printf("\t%u for %s.\n", move_index + 1, move_data->move_names[move_index]);
            sleep(0.3);
            }

        input_data->interval[0] = 1;
        input_data->interval[1] = 4;
        input_data->buffer_size = 2;
        
        rpse_io_int(input_data, false, "Select a move by it's number: ");
        printf("\n");
        round_info.p1_move = --input_data->input.int_input;
        
        printf("<--- DONE --->\n\n");

        printf("<---- BOT'S TURN ---->\n\n");

        srand(time(NULL));
        round_info.p2_move = rand() % 4;
        for (unsigned short int iteration = 0; iteration < 3; iteration++)
            {
            printf(". ");
            fflush(stdout);
            sleep(1);
            }
        
        printf("\n\n<--- DONE --->\n\n");

        rpse_roundCalc_getWinner(&round_info, move_data);

        player_data_t player_data =
        (player_data_t)
        {
            .PLAYER_1_NAME = "Player",
            .PLAYER_2_NAME = "Bot",
            .player_1_ready = false,
            .player_2_ready = true /* The bot will obviously always be ready */
        };

        rpse_sharedGamemodeMenus_roundSummary(&round_info, move_data, &player_data, 1);
        round_info.round_num++;

        if (round_info.p1_wins == 3 || round_info.p2_wins == 3)
            {
            switch (rpse_sharedGamemodeMenus_endOfGameMenu(input_data, true))
                {
                case 1:
                    rpse_roundCalc_prepNewMatch(&round_info);
                    break;

                case 2:
                    rpse_roundCalc_prepNewMatch(&round_info);
                    rpse_moveDef_redoMoves(input_data, move_data);
                    rpse_sharedGamemodeMenus_roundStartCountdown();
                    break;

                case 3:
                    rpse_moveDef_freeMoveData(move_data);
                    return 0;

                case 4:
                    round_info.replay = false;
                    break;
                }
            }
        }

    rpse_moveDef_freeMoveData(move_data);

    return 2;
}