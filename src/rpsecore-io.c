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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "../include/rpsecore-error.h"
#include "../include/rpsecore-io.h"

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
	static bool FIRST_CALL = true;

	if (FIRST_CALL == true)
		FIRST_CALL = false;
	else
		getchar();

	printf("Press enter to continue . . . ");
	getchar();	
	printf("\n");

	return 0;
}

/* Pointer must be freed by caller */
void *
rpse_io_threadedEnterToContinue(void)
{
	static bool FIRST_CALL = true;

	if (FIRST_CALL == true)
		FIRST_CALL = false;
	else
		getchar();

	printf("Press enter to continue . . . ");
	getchar();	
	printf("\n");

	unsigned short int *ret_val = malloc(sizeof(unsigned short int));
	rpse_error_checkuShortMalloc(ret_val);

	pthread_exit(ret_val);
}

void
rpse_io_str(user_input_data_t *input_data, bool insert_tab_before_input)
{
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);

	input_data->input.str_input = malloc(input_data->buffer_size);
	rpse_error_checkStringMalloc(input_data->input.str_input);

	getchar();

	fgets(input_data->input.str_input, input_data->buffer_size, stdin);

	input_data->input.str_input[strcspn(input_data->input.str_input, "\n")] = 0;

	input_data->input.str_input = realloc(input_data->input.str_input, strlen(input_data->input.str_input) + 1);
	rpse_error_checkStringMalloc(input_data->input.str_input);
}

void
rpse_io_int(user_input_data_t *input_data, bool insert_tab_before_input, char *prompt)
{
	if (input_data->interval[0] > input_data->interval[1])
		rpse_error_blameDev();
	
	printf("%s", prompt);
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);
	scanf(" %d", &input_data->input.int_input);

	while (input_data->input.int_input < input_data->interval[0] ||
		   input_data->input.int_input > input_data->interval[1])
		{
		getchar();
				
		fprintf(stderr, "Invalid input! Input must be a whole number in range of %d-%d.\n",
				input_data->interval[0], input_data->interval[1]);

		_rpse_io_static_tabBeforeInput(insert_tab_before_input);

		printf("%s", prompt);
		scanf(" %d", &input_data->input.int_input);
		}
}


// experimental function (does not even work yet!!)
/*
void rpse_io_int(user_input_data_t *input_data, bool insert_tab_before_input, char* prompt) {
	//use strtol eventually?
	bool input_is_valid = false;

	while (!input_is_valid) {
		printf("%s", prompt);
		rpse_io_str(input_data, insert_tab_before_input);
		unsigned short int inputted_str_len = strlen(input_data->input.str_input);

		for (unsigned short int index = 0; index < inputted_str_len; index++) {
			if (!isdigit(input_data->input.str_input[index])) {
				fprintf(stderr, "Invalid input! Input must be a whole number in range of %d-%d.\n", \
					input_data->interval[0], input_data->interval[1]);
				free(input_data->input.str_input);
				input_data->input.str_input = NULL;
			}
			else if (index == inputted_str_len - 1) {
				input_data->input.int_input = atoi(input_data->input.str_input);
				input_is_valid = (input_data->input.int_input > input_data->interval[0] && \
					input_data->input.int_input < input_data->interval[1]) ? true : false;
			}
		}
	}
}
*/


void
rpse_io_yn(user_input_data_t *input_data, bool insert_tab_before_input)
{
	_rpse_io_static_tabBeforeInput(insert_tab_before_input);
	
	input_data->input.char_input = tolower(getchar());

	while (input_data->input.char_input != 'y' && input_data->input.char_input != 'n')
		{
		getchar();
		fprintf(stderr, "Invalid input! Insert 'y' for yes or 'n' for no.\n");

		_rpse_io_static_tabBeforeInput(insert_tab_before_input);
		input_data->input.char_input = tolower(getchar());
		}
		
	getchar();
}