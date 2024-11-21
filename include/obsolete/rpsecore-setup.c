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
#include <stddef.h>
#include <string.h>
#include "../include/rpsecore-setup.h"

#define str_arr_null_check(arr, index) { \
	if (arr[index]==NULL) { \
		for (int iteration=0; iteration<index; iteration++) { \
			free(arr[iteration]); \
		} \
		free(arr); \
		exit(1); \
	} \
}

char** move_name_creation(union user_input *p_usr_in) {
	const size_t STD_MOVE_SIZES[3]={5, 6, 9};
	const char STD_MOVE_NAMES[3][9]={"Rock", "Paper", "Scissors"};
	char **mov_arr=malloc(sizeof(char*)*4);
	if (mov_arr==NULL) {
		fprintf(stderr, "Malloc() fail for \"**move_arr\" of type \"char**\".\n");
		exit(1);
	}
	for (int iteration=0; iteration<3; iteration++) {
		mov_arr[iteration]=malloc(STD_MOVE_SIZES[iteration]);
		str_arr_null_check(mov_arr, iteration);
		strcpy(mov_arr[iteration], STD_MOVE_NAMES[iteration]);
	}
	printf("\nWhen playing, you can play rock, paper, scissors or something else.\n");
	printf("Since you haven't done so earlier, you'll configure this \"something else\" now.\n");
	printf("[1/4] What should your custom move be named (30 char limit): ");
	str_input_prompt(p_usr_in, 31);
	mov_arr[3]=malloc(strlen(p_usr_in->str_in)+1);
	str_arr_null_check(mov_arr, 3);
	strcpy(mov_arr[3], p_usr_in->str_in);
	free(p_usr_in->str_in);
	return mov_arr;
}

void move_name_realloc(char **p_arr, union user_input *p_usr_in) {
	printf("[1/4] What should your custom move be renamed to (30 char limit): ");
	str_input_prompt(p_usr_in, 31);
	p_arr[3]=realloc(p_arr[3], (size_t)(strlen(p_usr_in->str_in)+1));
	strcpy(p_arr[3], p_usr_in->str_in);
	free(p_usr_in->str_in);
}

void move_4_combination_creation(struct move_4_combinations *p_m4_com, union user_input *p_usr_in) {
	p_m4_com->beats_rock=p_m4_com->beats_paper=p_m4_com->beats_scissors=0;	
	const char STD_MOVE_NAMES[3][9] = {"rock", "paper", "scissors"};
	int *m4_com_members[3]= {&p_m4_com->beats_rock, &p_m4_com->beats_paper, &p_m4_com->beats_scissors};
	for (int iteration=0; iteration<3; iteration++) {
		printf("[%d/4] Can your custom move beat %s? (Y/N): ", iteration+2, STD_MOVE_NAMES[iteration]);
		yes_no_prompt(p_usr_in);
		*m4_com_members[iteration] = (p_usr_in->char_in=='y') ? 1 : 0;
	}
}