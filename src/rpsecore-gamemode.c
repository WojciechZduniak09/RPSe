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

#include "../include/rpsecore-gamemode.h"
#include "../include/rpsecore-io.h"
#include "../include/rpsecore-logic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static void rpse_gamemode_static_roundStartCountdown(struct  timespec *ts) {
    printf("Great! The game will start in 3");
    fflush(stdout);
    rpse_logic_wait(1000, ts);
    for (unsigned short int countdown_number = 2; countdown_number > 0; countdown_number--) {
        printf(", %u", countdown_number);
        fflush(stdout);
        rpse_logic_wait(1000, ts);
    }
    printf(", 0.\n");
    rpse_logic_wait(1000, ts);
}

unsigned short int rpse_gamemode_menu(user_input_data_t *input_data) {
    printf("<----- Gamemode menu ----->\n"
            "1. Player vs Player (PvP - WIP).\n"
            "2. Player vs Bot (PvE).\n\n");

    input_data->interval[0] = 1;
    input_data->interval [1] = 2;
    input_data->buffer_size = 2;
    
    rpse_io_int(input_data, false, "Select a gamemode by it's number: ");

    return input_data->input.int_input;
}

unsigned short int rpse_gamemode_pvp(struct timespec *ts) {
    printf("This gamemode is still a work in progress, please try again in a future update.\n");

    rpse_logic_wait(3000, ts);
    return 0;
}

unsigned short int rpse_gamemode_pve(user_input_data_t *input_data, struct timespec *ts) {
    printf("\nWelcome to PvE! Before playing, you'll have to set up a few things...\n");

    move_data_t *move_data = rpse_logic_setUpMoves(input_data, ts);

    rpse_gamemode_static_roundStartCountdown(ts);

    round_info_t round_info = {
        .winner = "n/a",
        .p1_wins = 0,
        .p1_move = 0,
        .p2_wins = 0,
        .p2_move = 0,
        .replay = true,
        .round_num = 1
    };

    while (round_info.replay == true && (round_info.p1_wins < 3 || round_info.p2_wins < 3)) {
        printf("\n");
        printf("<----- ROUND %u ----->\n\n", round_info.round_num);
        rpse_logic_wait(500, ts);
        printf("<---- PLAYER'S TURN ---->\n");
        rpse_logic_wait(500, ts);
        printf("<--- Move options --->\n");
        for (unsigned short int move_index = 0; move_index < 4; move_index++) {
            printf("\t%u for %s.\n", move_index + 1, move_data->move_names[move_index]);
            rpse_logic_wait(300, ts);
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
        for (unsigned short int iteration = 0; iteration < 3; iteration++) {
            printf(". ");
            fflush(stdout);
            rpse_logic_wait(670, ts);
        }
        printf("\n\n<--- DONE --->\n\n");

        rpse_logic_getWinner(&round_info, move_data);

        char PLAYER_NAMES[2][31] = {{"Player"}, {"Bot"}};
        rpse_logic_roundSummary(&round_info, move_data, PLAYER_NAMES, ts);
        round_info.round_num++;

        if (round_info.p1_wins == 3 || round_info.p2_wins == 3) {
            switch(rpse_logic_endOfGameMenu(input_data, ts)) {
                case 1:
                    rpse_logic_prepNewMatch(&round_info);
                    break;
                case 2:
                    rpse_logic_prepNewMatch(&round_info);
                    rpse_logic_redoMoves(input_data, move_data);
                    rpse_gamemode_static_roundStartCountdown(ts);
                    break;
                case 3:
                    rpse_logic_freeMoveData(move_data);

                    return 0;
                case 4:
                    round_info.replay = false;
                    break;
            }
        }
    }
    rpse_logic_freeMoveData(move_data);

    return 2;
}