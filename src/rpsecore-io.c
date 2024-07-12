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
#include "../include/rpsecore-error.h"
#include "../include/rpsecore-io.h"

void rpse_io_static_tabBeforeInput(bool insertTabBeforeInput) {
	if (insertTabBeforeInput == true) {
		printf("\t");
	}
}

void rpse_io_static_clearStdin(void) {
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {};
}

void rpse_io_static_confirmFirstCall(unsigned short int target) {
	if (target == 0) {
		rpse_io_static_clearStdin();
	}
	else {
		target--;
	}

}

void rpse_io_enterToContinue(void) {
	/* FIRST_CALL is used as a boolean value (0 == false, anything but 0 == true) */
	static unsigned short int FIRST_CALL = 1;
	rpse_io_static_confirmFirstCall(FIRST_CALL);

	printf("Press enter to continue . . . ");
	getchar();	
	printf("\n");
}

void rpse_io_str(user_input_data_t *input_data, bool insert_tab_before_input) {
	rpse_io_static_tabBeforeInput(insert_tab_before_input);

	input_data->input.str_input = malloc(input_data->buffer_size);
	rpse_error_checkStringMalloc(input_data->input.str_input);

	getchar();

	fgets(input_data->input.str_input, input_data->buffer_size, stdin);

	input_data->input.str_input[strcspn(input_data->input.str_input, "\n")] = 0;

	input_data->input.str_input = realloc(input_data->input.str_input, strlen(input_data->input.str_input) + 1);
	rpse_error_checkStringMalloc(input_data->input.str_input);
}

void rpse_io_int(user_input_data_t *input_data, bool insert_tab_before_input, char* prompt) {
	if (input_data->interval[0] > input_data->interval[1]) {
		rpse_error_blameDev();
	}

	rpse_io_static_tabBeforeInput(insert_tab_before_input);

	printf("%s", prompt);
	scanf(" %d", &input_data->input.int_input);

	while (input_data->input.int_input < input_data->interval[0] || \
			input_data->input.int_input > input_data->interval[1]) {
				getchar();
				
				fprintf(stderr, "Invalid input! Input must be a whole number in range of %d-%d.\n", \
						input_data->interval[0], input_data->interval[1]);

				rpse_io_static_tabBeforeInput(insert_tab_before_input);

				printf("%s", prompt);
				scanf(" %d", &input_data->input.int_input);
	}
}

void rpse_io_yn(user_input_data_t *input_data, bool insert_tab_before_input) {
	/* ffs fix this bloody shitshow of a function */
	rpse_io_static_tabBeforeInput(insert_tab_before_input);
	
	input_data->input.char_input = tolower(getchar());

	while (input_data->input.char_input != 'y' && input_data->input.char_input != 'n') {
		fprintf(stderr, "Invalid input! Insert 'y' for yes or 'n' for no.\n");

		rpse_io_static_tabBeforeInput(insert_tab_before_input);
		input_data->input.char_input = tolower(getchar());

		rpse_io_static_clearStdin();
	}
}