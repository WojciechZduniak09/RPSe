/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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

static unsigned short int
_rpse_moveDef_makeCustomMoveRelations(user_input_data_t *input_data, move_data_t *move_data)
{
	if (input_data == NULL)
		{
		perror("_rpse_moveDef_makeCustomMoveRelations() --> input_data == NULL");
		return EXIT_FAILURE;
		}

	else if (move_data == NULL)
		{
		perror("_rpse_moveDef_makeCustomMoveRelations() --> move_data == NULL");
		return EXIT_FAILURE;
		}
	
	unsigned short int array_index;

	for (unsigned short int move_array_index = 0; move_array_index < 3; move_array_index++)
		{
		printf("[%u/4] Can %s beat %s? (y/n)\n", move_array_index + 2, move_data->move_names[3],
			   move_data->move_names[move_array_index]);
		
		if (rpse_io_yn(input_data, true) == EXIT_FAILURE)
			{
			perror("_rpse_moveDef_makeCustomMoveRelations() --> rpse_io_yn() == EXIT_FAILURE");
			return EXIT_FAILURE;
			}

		array_index = move_array_index;
		if (input_data->input.char_input == 'y')
			{
			move_data->winning_combinations[array_index + 3][0] = 3;
			move_data->winning_combinations[array_index + 3][1] = move_array_index;
			}

		else
			{
			move_data->winning_combinations[array_index + 3][0] = move_array_index;
			move_data->winning_combinations[array_index + 3][1] = 3;
			}
		}

	return EXIT_SUCCESS;
}

/*
=======================
MOVE-DEFINING FUNCTIONS
=======================
*/

/* Returns NULL for errors */
move_data_t *
rpse_moveDef_setUpMoves(user_input_data_t *input_data)
{
	if (input_data == NULL)
		{
		perror("rpse_moveDef_setUpMoves() --> input_data == NULL");
		return NULL;
		}

	_rpse_moveDef_customMoveExplanation();
	
	move_data_t *new_move_data = NULL;

	for (unsigned short int attempt = 1; attempt <= 3 && new_move_data == NULL; attempt++)
		new_move_data = calloc(1, sizeof(move_data_t));
	
	if (new_move_data == NULL)
		{
		perror("rpse_moveDef_setUpMoves() --> new_move_data == NULL");
		return NULL;
		}

	new_move_data->move_names = NULL;
	
	for (unsigned short int attempt = 1; attempt <= 3 && new_move_data->move_names == NULL; attempt++)
		new_move_data->move_names = calloc(4, sizeof(char *));

	if (new_move_data->move_names == NULL)
		{
		perror("rpse_moveDef_setUpMoves() --> new_move_data->move_names == NULL");
		return NULL;
		}

	const char STD_MOVE_NAMES[3][9] = {"rock", "paper", "scissors"};

	for (unsigned short int move_arr_index = 0; move_arr_index < 3; move_arr_index++)
		{
		for (unsigned short int attempt = 1; attempt <= 3 && new_move_data->move_names[move_arr_index] == NULL; attempt++)
			new_move_data->move_names[move_arr_index] = calloc(strlen(STD_MOVE_NAMES[move_arr_index]) + 1, sizeof(char));

		if (new_move_data->move_names[move_arr_index] == NULL)
			{
			perror("rpse_moveDef_setUpMoves() --> new_move_data->move_names[move_arr_index] == NULL");
			return NULL;
			}

		strncpy(new_move_data->move_names[move_arr_index], STD_MOVE_NAMES[move_arr_index], strlen(STD_MOVE_NAMES[move_arr_index]) + 1);
		}

	const unsigned short int STD_MOVE_COMBINATIONS[3][2] = {{0, 2}, {1, 0}, {2, 1}};

	for (unsigned short int move_array_index = 0; move_array_index < 3; move_array_index++)
		{
		for (unsigned short int move_index = 0; move_index < 2; move_index++)
			new_move_data->winning_combinations[move_array_index][move_index] = STD_MOVE_COMBINATIONS[move_array_index][move_index];
		}
	bool move_name_is_valid = false;
	unsigned short int name_size;
	while (!move_name_is_valid)
	    {
	    /* Setting up custom move - the limitations are due to the UDP broadcast aspect only */
	    printf("[1/4] What should your custom move be named? (30 character limit and no parentheses allowed)\n");
	    input_data->buffer_size = 31;

	    if (rpse_io_str(input_data, true) == EXIT_FAILURE)
	       	    {
		    perror("rpse_moveDef_setUpMoves() --> rpse_io_str() == EXIT_FAILURE");
		    return NULL;
		    }

	    name_size = strlen(input_data->input.str_input) + 1;
	    unsigned short int current_character_index = 0;
	    char current_character;
	    do
	        {
		if (strcmp(input_data->input.str_input, "") == EXIT_SUCCESS)
			{
			printf("Invalid name! You must not give an empty name.\n");
			move_name_is_valid = false;
			break;
			}
	        current_character = input_data->input.str_input[current_character_index];
	        if (current_character == '(' || current_character == ')')
			{
			printf("Invalid name! You must not include the symbols \"(\" and \")\".\n");
			move_name_is_valid = false;
			break;
			}
		else
			move_name_is_valid = true;
		current_character_index++;
		}
	    while (current_character_index < name_size);
	    }

	for (unsigned short int attempt = 1; attempt <= 3 && new_move_data->move_names[3] == NULL; attempt++)
		new_move_data->move_names[3] = calloc(name_size, sizeof(char));

	if (new_move_data->move_names[3] == NULL)
		{
		perror("rpse_moveDef_setUpMoves() --> new_move_data->move_names[3] == NULL");
		return NULL;
		}

	strncpy(new_move_data->move_names[3], input_data->input.str_input, name_size);

	memset(input_data->input.str_input, 0, name_size);
	free(input_data->input.str_input);
	input_data->input.str_input = NULL;
	
	_rpse_moveDef_makeCustomMoveRelations(input_data, new_move_data);

	printf("\n");

	return new_move_data;
}

unsigned short int
rpse_moveDef_redoCustomMove(user_input_data_t *input_data, move_data_t *move_data)
{
	if (input_data == NULL)
		{
		perror("rpse_moveDef_redoCustomMove() --> input_data == NULL");
		return EXIT_FAILURE;
		}
	else if (move_data == NULL)
		{
		perror("rpse_moveDef_redoCustomMove() --> move_data == NULL");
		return EXIT_FAILURE;
		}
	
	printf("What should %s be renamed to? (30 character limit)\n", move_data->move_names[3]);
	
	input_data->buffer_size = 31;
	if (rpse_io_str(input_data, true) == EXIT_FAILURE)
		{
		perror("rpse_moveDef_redoCustomMove() --> rpse_io_str() == EXIT_FAILURE");
		return EXIT_FAILURE;
		}
	
	unsigned short int new_name_size = strlen(input_data->input.str_input) + 1;

	for (unsigned short int attempt = 1; attempt <= 3 && move_data->move_names[3] == NULL; attempt++)
		move_data->move_names[3] = realloc(move_data->move_names[3], new_name_size);
		
	if (move_data->move_names[3] == NULL)
		{
		perror("rpse_moveDef_redoCustomMove() --> move_data->move_names[3] == NULL");
		return EXIT_FAILURE;
		}

	strncpy(move_data->move_names[3], input_data->input.str_input, (size_t)new_name_size);
	free(input_data->input.str_input);
	input_data->input.str_input = NULL;

	_rpse_moveDef_makeCustomMoveRelations(input_data, move_data);

	return EXIT_SUCCESS;
}

/*
================
CLEANUP FUNCTION
================
*/

/* Keep all strings in array defined before using */
unsigned short int
rpse_moveDef_freeMoveData(move_data_t *move_data)
{
	if (move_data == NULL)
		{
		perror("rpse_moveDef_freeMoveData() --> move_data == NULL");
		return EXIT_FAILURE;
		}
	
    for (unsigned short int array_index = 0; array_index < 4; array_index++)
		{
		if (move_data->move_names[array_index] == NULL)
			{
			perror("rpse_moveDef_freeMoveData() --> move_data->move_names[array_index] == NULL");
			return EXIT_FAILURE;
			}
		
        free(move_data->move_names[array_index]);
        move_data->move_names[array_index] = NULL;
    	}
	
    free(move_data->move_names);
    move_data->move_names = NULL;
    
    free(move_data);
    move_data = NULL;
    return EXIT_SUCCESS;
}
