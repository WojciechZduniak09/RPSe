/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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
#include <stdio.h>
#include <stdlib.h>

/*
=================
WINNER CALCULATOR
=================
*/

unsigned short int
rpse_roundCalc_getWinner(round_info_t *round_info, move_data_t *move_data) {
	if (round_info == NULL)
		{
		perror("\"round_info == NULL\" while attempting to calculate winner");
		return EXIT_FAILURE;
		}
	
	else if (move_data == NULL)
		{
		perror("\"move_data == NULL\" while attempting to calculate winner");
		return EXIT_FAILURE;
		}

	bool player1_element_match, player2_element_match;
	unsigned short int player1_combination[2] = {round_info->p1_move, round_info->p2_move};
	unsigned short int player2_combination[2] = {round_info->p2_move, round_info->p1_move};

	memset(round_info->winner, 0, 7);
	strncpy(round_info->winner, "", strlen("") + 1);

	for (unsigned short int combination_array_index = 0; 
		 combination_array_index < 6 && strncmp(round_info->winner, "", 7) == 0; 
		 combination_array_index++) 
		{
		player1_element_match = (player1_combination[0] == move_data->winning_combinations[combination_array_index][0] && \
			player1_combination[1] == move_data->winning_combinations[combination_array_index][1]);
		
		player2_element_match = (player2_combination[0] == move_data->winning_combinations[combination_array_index][0] && \
			player2_combination[1] == move_data->winning_combinations[combination_array_index][1]);
		
		if (player1_element_match) 
			{
			round_info->p1_wins++;
			strncpy(round_info->winner, "p1", strlen("p1") + 1);
			}
		else if (player2_element_match) 
			{
			round_info->p2_wins++;
			strncpy(round_info->winner, "p2", strlen("p2") + 1);
			}
		else if (!player1_element_match && !player2_element_match && combination_array_index == 5)
			strncpy(round_info->winner, "NOBODY", strlen("NOBODY") + 1);
		}
	
	return EXIT_SUCCESS;
}

/*
==========================
ROUND PREPARATION FUNCTION
==========================
*/

unsigned short int
rpse_roundCalc_prepNewMatch(round_info_t *round_info) {
	if (round_info == NULL)
		{
		perror("\"round_info == NULL\" while attempting to prep new match");
		return EXIT_FAILURE;
		}
	
	round_info->p1_wins = round_info->p2_wins = 0;
	round_info->round_num = 1;

	return EXIT_SUCCESS;
}