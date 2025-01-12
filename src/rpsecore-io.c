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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include  <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "../include/rpsecore-io.h"

bool enter_to_continue_first_call = true;
pthread_mutex_t enter_to_continue_lock = PTHREAD_MUTEX_INITIALIZER;

/*
================
STATIC FUNCTIONS
================
*/

static void
_rpse_io_static_tabBeforeInput(bool insertTabBeforeInput)
{
	if (insertTabBeforeInput == true)
		printf("\t");
}

/*
===============
INPUT FUNCTIONS
===============
*/

unsigned short int
rpse_io_enterToContinue(void)
{
	if (enter_to_continue_first_call)
		enter_to_continue_first_call = false;
	else
		getchar();
	
	printf("Press enter to continue . . . ");
	getchar();	
	printf("\n");

	return EXIT_SUCCESS;
}

/* No arguments truly needed */
void *
rpse_io_threadedEnterToContinue(void *arg)
{
	pthread_mutex_lock(&enter_to_continue_lock);
	if (enter_to_continue_first_call)
		enter_to_continue_first_call = false;
	else
		getchar();
		
	printf("Press enter to continue . . . ");
	getchar();
	printf("\n");

	pthread_mutex_unlock(&enter_to_continue_lock);

	pthread_exit(NULL);
	return NULL;
}

/* String must be freed by caller before using again and at end of code */
unsigned short int
rpse_io_str(user_input_data_t *input_data, bool insert_tab_before_input)
{
	static bool first_time_called = true;
	if (input_data == NULL)
		{
		perror("rpse_io_str() --> input_data == NULL");
		return EXIT_FAILURE;
		}
	
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);
	
	input_data->input.str_input = NULL;

	for (unsigned short int attempt = 1; attempt <= 3 && input_data->input.str_input == NULL; attempt++)
			input_data->input.str_input = calloc(input_data->buffer_size, sizeof(char));

	if (input_data->input.str_input == NULL)
		{
		perror("rpse_io_str() --> BEFORE fgets() --> input_data->input.str_input == NULL");
		return EXIT_FAILURE;
		}

	if (first_time_called)
		getchar();

	fgets(input_data->input.str_input, input_data->buffer_size, stdin);

	input_data->input.str_input[strcspn(input_data->input.str_input, "\n")] = 0;

	for (unsigned short int attempt = 1; attempt <= 3 && input_data->input.str_input == NULL; attempt++)
		input_data->input.str_input = realloc(input_data->input.str_input, strlen(input_data->input.str_input) + 1);
	
	first_time_called = false;

	if (input_data->input.str_input == NULL)
		{
		perror("rpse_io_str() --> AFTER fgets() --> input_data->input.str_input == NULL");
		return EXIT_FAILURE;
		}
	else
		return EXIT_SUCCESS;
}

unsigned short int
rpse_io_int(user_input_data_t *input_data, bool insert_tab_before_input, char *prompt)
{
	if (input_data == NULL)
		{
		perror("rpse_io_int() --> input_data == NULL");
		return EXIT_FAILURE;
		}
	
	else if (input_data->interval[0] > input_data->interval[1])
		{
		perror("rpse_io_int() --> input_data->interval[0] > input_data->interval[1]");
		return EXIT_FAILURE;
		}
	
	printf("%s", prompt);
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);
	scanf(" %d", &input_data->input.int_input);

	while (input_data->input.int_input < input_data->interval[0] || input_data->input.int_input > input_data->interval[1])
		{
		getchar();
				
		fprintf(stderr, "Invalid input! Input must be a whole number in range of %d-%d.\n",
				input_data->interval[0], input_data->interval[1]);

		_rpse_io_static_tabBeforeInput(insert_tab_before_input);

		printf("%s", prompt);
		scanf(" %d", &input_data->input.int_input);
		}

	return EXIT_SUCCESS;
}

unsigned short int
rpse_io_yn(user_input_data_t *input_data, bool insert_tab_before_input)
{
	if (input_data == NULL)
		{
		perror("rpse_io_yn() --> input_data == NULL");
		return EXIT_FAILURE;
		}
	
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);
	
	input_data->input.char_input = tolower(getchar());

	while (input_data->input.char_input != 'y' && input_data->input.char_input != 'n')
		{
		getchar();
		fprintf(stderr, "Invalid input! Insert 'y' for yes or 'n' for no.\n");

		fflush(stdin);
		
		_rpse_io_static_tabBeforeInput(insert_tab_before_input);
		input_data->input.char_input = tolower(getchar());
		}
		
	getchar();

	return EXIT_SUCCESS;
}
