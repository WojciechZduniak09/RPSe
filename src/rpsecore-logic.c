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

#include "../include/rpsecore-io.h"
#include "../include/rpsecore-logic.h"
#include "../include/rpsecore-error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

void rpse_logic_wait(long delay_in_milliseconds, struct timespec *ts) {
	if (delay_in_milliseconds < 0) {
		rpse_error_blameDev();
	}

	ts->tv_sec = delay_in_milliseconds / 1000;
	ts->tv_nsec = (delay_in_milliseconds % 1000) * 1000000;

	int result;
	do {
		result = nanosleep(ts, ts);
	}
	while(result && errno == EINTR);
}

static void rpse_logic_static_customMoveExplanation(struct timespec *ts) {
	printf("\nDuring games, you can play with:\n");
	fflush(stdout);
	rpse_logic_wait(300, ts);

	printf("\t1. Rock.\n");
	fflush(stdout);
	rpse_logic_wait(300, ts);

	printf("\t2. Paper.\n");
	fflush(stdout);
	rpse_logic_wait(300, ts);

	printf("\t3. Scissors.\n");
	fflush(stdout);
	rpse_logic_wait(300, ts);

	printf("\t4. A custom move.\n");
	fflush(stdout);
	rpse_logic_wait(300, ts);

	printf("\nYou'll set up this custom move now.\n");
	fflush(stdout);
	rpse_logic_wait(2000, ts);
}

static void rpse_logic_static_makeCustomMoveRelations(user_input_data_t *input_data, move_data_t *move_data) {
	unsigned short int array_index;
	for (unsigned short int move_index = 0; move_index < 3; move_index++) {
		printf("[%u/4] Can %s beat %s? (y/n)\n", move_index + 2, move_data->move_names[3], \
			move_data->move_names[move_index]);
		rpse_io_yn(input_data, true);

		array_index = move_index;
		if (input_data->input.char_input == 'y') {
			move_data->winning_combinations[array_index + 3][0] = 3;
			move_data->winning_combinations[array_index + 3][1] = move_index;
		}
		else {
			move_data->winning_combinations[array_index + 3][0] = move_index;
			move_data->winning_combinations[array_index + 3][1] = 3;
		}
	}
}

move_data_t* rpse_logic_setUpMoves(user_input_data_t *input_data, struct timespec *ts) {
	rpse_logic_static_customMoveExplanation(ts);
	
	move_data_t *new_move_data = malloc(sizeof(move_data_t));
	rpse_error_checkMoveDataMalloc(new_move_data);

	new_move_data->move_names = malloc(sizeof(char*) * 4);
	rpse_error_checkStringArrayMalloc(new_move_data->move_names);

	const char STD_MOVE_NAMES[3][9] = {"rock", "paper", "scissors"};
	for (unsigned short int move_index = 0; move_index < 3; move_index++) {
		new_move_data->move_names[move_index] = malloc(strlen(STD_MOVE_NAMES[move_index]) + 1);
		rpse_error_checkStringMalloc(new_move_data->move_names[move_index]);

		
		strcpy(new_move_data->move_names[move_index], STD_MOVE_NAMES[move_index]);
	}

	const unsigned short int STD_MOVE_COMBINATIONS[3][2] = {{0, 2}, {1, 0}, {2, 1}};
	for (unsigned short int array_index = 0; array_index < 3; array_index++) {
		for (unsigned short int element_index = 0; element_index < 2; element_index++) {
			new_move_data->winning_combinations[array_index][element_index] = \
				STD_MOVE_COMBINATIONS[array_index][element_index];
		}
	}
	
	printf("[1/4] What should your custom move be named? (30 character limit)\n");
	input_data->buffer_size = 31;
	rpse_io_str(input_data, true);
	unsigned short int name_size = strlen(input_data->input.str_input) + 1;

	new_move_data->move_names[3] = malloc(name_size);
	rpse_error_checkStringMalloc(new_move_data->move_names[3]);

	strcpy(new_move_data->move_names[3], input_data->input.str_input);
	free(input_data->input.str_input);
	input_data->input.str_input = NULL;
	
	rpse_logic_static_makeCustomMoveRelations(input_data, new_move_data);

	printf("\n");

	return new_move_data;
}

void rpse_logic_redoMoves(user_input_data_t *input_data, move_data_t *move_data) {
	printf("What should %s be renamed to? (30 character limit)\n", move_data->move_names[3]);
	
	input_data->buffer_size = 31;
	rpse_io_str(input_data, true);
	unsigned short int new_name_size = strlen(input_data->input.str_input) + 1;

	move_data->move_names[3] = realloc(move_data->move_names[3], new_name_size);
	rpse_error_checkStringMalloc(move_data->move_names[3]);

	strcpy(move_data->move_names[3], input_data->input.str_input);
	free(input_data->input.str_input);
	input_data->input.str_input = NULL;

	rpse_logic_static_makeCustomMoveRelations(input_data, move_data);
}

void rpse_logic_freeMoveData(move_data_t *move_data) {
    for (unsigned short int array_index = 0; array_index < 4; array_index++) {
        free(move_data->move_names[array_index]);
        move_data->move_names[array_index] = NULL;
    }
    free(move_data->move_names);
    move_data->move_names = NULL;
    
    free(move_data);
    move_data = NULL;
}

void rpse_logic_getWinner(round_info_t *round_info, move_data_t *move_data) {
	bool player1_element_match, player2_element_match;
	unsigned short int player1_combination[2] = {round_info->p1_move, round_info->p2_move};
	unsigned short int player2_combination[2] = {round_info->p2_move, round_info->p1_move};

	for (unsigned short int array_index = 0; array_index < 6; array_index++) {
		player1_element_match = (player1_combination[0] == move_data->winning_combinations[array_index][0] && \
			player1_combination[1] == move_data->winning_combinations[array_index][1]);
		
		player2_element_match = (player2_combination[0] == move_data->winning_combinations[array_index][0] && \
			player2_combination[1] == move_data->winning_combinations[array_index][1]);
		
		if (player1_element_match) {
			round_info->p1_wins++;
			strcpy(round_info->winner, "p1");
			break;
		}
		else if (player2_element_match) {
			round_info->p2_wins++;
			strcpy(round_info->winner, "p2");
			break;
		}
		else if (!player1_element_match && !player2_element_match && array_index == 5) {
			strcpy(round_info->winner, "NOBODY");
		}
	}
}

void rpse_logic_roundSummary(round_info_t *round_info, move_data_t *move_data, char player_names[2][31], \
struct timespec *ts) {
	if (strncmp(round_info->winner, "p1", 3) == 0) {
        strcpy(round_info->winner, player_names[0]);
    }
    else if (strncmp(round_info->winner, "p2", 3) == 0) {
        strcpy(round_info->winner, player_names[1]);
    }
	
	printf("<----- ROUND RESULTS ----->\n");
	rpse_logic_wait(350, ts);
	printf("Winner: %s.\n", round_info->winner);
	rpse_logic_wait(350, ts);
	printf("%s's info:\n", player_names[0]);
	rpse_logic_wait(350, ts);
	printf("\tSelected move: %s.\n", move_data->move_names[round_info->p1_move]);
	rpse_logic_wait(350, ts);
	printf("\tCurrent win count: %u/3.\n", round_info->p1_wins);
	rpse_logic_wait(350, ts);

	printf("%s's info:\n", player_names[1]);
	rpse_logic_wait(350, ts);
	printf("\tSelected move: %s.\n", move_data->move_names[round_info->p2_move]);
	rpse_logic_wait(350, ts);
	printf("\tCurrent win count: %u/3.\n\n", round_info->p2_wins);
	rpse_logic_wait(350, ts);

	rpse_io_enterToContinue();
}

unsigned short int rpse_logic_endOfGameMenu(user_input_data_t *input_data, struct timespec *ts) {
	const char MENU_OPTIONS[4][28] = {"Replay", "Edit custom move & replay", "Change gamemode", "Exit game"};

	printf("<----- END OF GAME MENU ----->\n");
	rpse_logic_wait(500, ts);
	for (unsigned short int menu_element_index = 0; menu_element_index < 4; menu_element_index++) {
		rpse_logic_wait(500, ts);
		printf("\t%u. %s.\n", menu_element_index + 1, MENU_OPTIONS[menu_element_index]);
	}
	input_data->interval[0] = 1;
	input_data->interval[1] = 4;
	rpse_io_int(input_data, false, "Choose an option by it's number: ");

	return input_data->input.int_input;
}

void rpse_logic_prepNewMatch(round_info_t *round_info) {
	round_info->p1_wins = round_info->p2_wins = 0;
	round_info->round_num = 1;
}