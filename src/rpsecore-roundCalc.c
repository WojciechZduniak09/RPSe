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

#include "../include/rpsecore-roundCalc.h"
#include "../include/rpsecore-moveDef.h"
#include <string.h>

/*
=================
WINNER CALCULATOR
=================
*/

void rpse_roundCalc_getWinner(round_info_t *round_info, move_data_t *move_data) {
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

/*
==========================
ROUND PREPARATION FUNCTION
==========================
*/

void rpse_roundCalc_prepNewMatch(round_info_t *round_info) {
	round_info->p1_wins = round_info->p2_wins = 0;
	round_info->round_num = 1;
}