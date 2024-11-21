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

#include "../include/rpsecore-moveDef.h"
#include "../include/rpsecore-io.h"
#include "../include/rpsecore-error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/*
================
STATIC FUNCTIONS
================
*/

static void
_rpse_moveDef_customMoveExplanation(void)
{
	printf("\nDuring games, you can play with:\n");
	fflush(stdout);
	sleep(0.3);

	printf("\t1. Rock.\n");
	fflush(stdout);
	sleep(0.3);

	printf("\t2. Paper.\n");
	fflush(stdout);
	sleep(0.3);

	printf("\t3. Scissors.\n");
	fflush(stdout);
	sleep(0.3);

	printf("\t4. A custom move.\n");
	fflush(stdout);
	sleep(0.3);

	printf("\nYou'll set up this custom move now.\n");
	fflush(stdout);
	sleep(0.2);
}

static void
rpse_moveDef_static_makeCustomMoveRelations(user_input_data_t *input_data, move_data_t *move_data)
{
	unsigned short int array_index;

	for (unsigned short int move_index = 0; move_index < 3; move_index++)
		{
		printf("[%u/4] Can %s beat %s? (y/n)\n", move_index + 2, move_data->move_names[3],
			   move_data->move_names[move_index]);
		rpse_io_yn(input_data, true);

		array_index = move_index;
		if (input_data->input.char_input == 'y')
			{
			move_data->winning_combinations[array_index + 3][0] = 3;
			move_data->winning_combinations[array_index + 3][1] = move_index;
			}
		else
			{
			move_data->winning_combinations[array_index + 3][0] = move_index;
			move_data->winning_combinations[array_index + 3][1] = 3;
			}
	}
}

/*
=======================
MOVE-DEFINING FUNCTIONS
=======================
*/

move_data_t *
rpse_moveDef_setUpMoves(user_input_data_t *input_data)
{
	_rpse_moveDef_customMoveExplanation();
	
	move_data_t *new_move_data = malloc(sizeof(move_data_t));
	rpse_error_checkMoveDataMalloc(new_move_data);

	new_move_data->move_names = malloc(sizeof(char *) * 4);
	rpse_error_checkStringArrayMalloc(new_move_data->move_names);

	const char STD_MOVE_NAMES[3][9] = {"rock", "paper", "scissors"};

	for (unsigned short int move_index = 0; move_index < 3; move_index++)
		{
		new_move_data->move_names[move_index] = malloc(strlen(STD_MOVE_NAMES[move_index]) + 1);
		rpse_error_checkStringMalloc(new_move_data->move_names[move_index]);

		strcpy(new_move_data->move_names[move_index], STD_MOVE_NAMES[move_index]);
		}

	const unsigned short int STD_MOVE_COMBINATIONS[3][2] = {{0, 2}, {1, 0}, {2, 1}};

	for (unsigned short int array_index = 0; array_index < 3; array_index++)
		{
		for (unsigned short int element_index = 0; element_index < 2; element_index++)
			{
			new_move_data->winning_combinations[array_index][element_index] =
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
	
	rpse_moveDef_static_makeCustomMoveRelations(input_data, new_move_data);

	printf("\n");

	return new_move_data;
}

void
rpse_moveDef_redoMoves(user_input_data_t *input_data, move_data_t *move_data)
{
	printf("What should %s be renamed to? (30 character limit)\n", move_data->move_names[3]);
	
	input_data->buffer_size = 31;
	rpse_io_str(input_data, true);
	unsigned short int new_name_size = strlen(input_data->input.str_input) + 1;

	move_data->move_names[3] = realloc(move_data->move_names[3], new_name_size);
	rpse_error_checkStringMalloc(move_data->move_names[3]);

	strcpy(move_data->move_names[3], input_data->input.str_input);
	free(input_data->input.str_input);
	input_data->input.str_input = NULL;

	rpse_moveDef_static_makeCustomMoveRelations(input_data, move_data);
}

/*
================
CLEANUP FUNCTION
================
*/

void
rpse_moveDef_freeMoveData(move_data_t *move_data)
{
    for (unsigned short int array_index = 0; array_index < 4; array_index++)
		{
        free(move_data->move_names[array_index]);
        move_data->move_names[array_index] = NULL;
    	}
	
    free(move_data->move_names);
    move_data->move_names = NULL;
    
    free(move_data);
    move_data = NULL;
}