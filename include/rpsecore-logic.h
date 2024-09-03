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

#ifndef RPSECORE_LOGIC_H
#define RPSECORE_LOGIC_H

#include <stdbool.h>
#include <time.h>
#include "rpsecore-io.h"

// Round information struct.
typedef struct {
	char winner[7];
	unsigned short int round_num;
	unsigned short int p1_wins;
	unsigned short int p1_move;
	unsigned short int p2_wins;
	unsigned short int p2_move;
	bool replay;
} round_info_t;

typedef struct {
	char **move_names;
	unsigned short int winning_combinations[6][2];
} move_data_t;

void rpse_logic_wait(long delay_in_milliseconds, struct timespec *ts);
move_data_t* rpse_logic_setUpMoves(user_input_data_t *input_data, struct timespec *ts);
void rpse_logic_redoMoves(user_input_data_t *input_data, move_data_t *move_data);
void rpse_logic_freeMoveData(move_data_t *move_data);
void rpse_logic_getWinner(round_info_t *round_info, move_data_t *move_data);
/* void rpse_logic_freeWinner(round_info_t *round_info); */
void rpse_logic_roundSummary(round_info_t *round_info, move_data_t *move_data, char player_names[2][31], \
	struct timespec *ts);
unsigned short int rpse_logic_endOfGameMenu(user_input_data_t *input_data, struct timespec *ts);
void rpse_logic_prepNewMatch(round_info_t *round_info);

#endif

/*
// Linked list functions.
node* rpse_logic_makeNewNode(unsigned short int data[2]);
void rpse_logic_addNodeAtBeginning(node **head, unsigned short int data[2]);
void rpse_logic_addNodeAtEnd(node **head, unsigned short int data[2]);
void rpse_logic_removeNode(node **head, node *target_node);

// Setup functions.
struct move_data rpse_logic_makeMoves(user_input_data *input_data);
*/


































// struct winning_combinations rpse_logic_makeMoveCombinations(struct user_input_data *input_data);


/*
void check_round_winner(struct round_info *p_rnd_inf, struct move_4_combinations *p_m4_com);
char** move_name_creation(union user_input *p_union);
void move_name_realloc(char **p_move_arr, union user_input *p_usr_in);
void move_4_combination_creation(struct move_4_combinations *p_m4_com, union user_input *p_usr_in);
*/