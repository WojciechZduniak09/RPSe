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
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>
#include "../include/rpsecore-g2.h"
/* Some could call this macro bloat, I call it a "mach-jesus-speed slower for non-robotic viewing".
 * That sounded like something Mark Zuckerberg would say to be honest.
 * Now just seriously, the macro is just so you can read text at a more natural pace.
 */
#define tmp_sleep(msec) { \
	int result=usleep(msec); \
	if (result!=0) { \
		fprintf(stderr, "Interruption detected during \"usleep()\".\n"); \
		exit(1); \
	} \
}

#define free_str_arr(arr, max_index) { \
	for (int iteration=0; iteration<max_index; iteration++) { \
		free(arr[iteration]); \
	} \
	free(arr); \
	arr=NULL; \
}

int gamemode_2(union user_input *p_usr_in) {
	const char MENU_OPTIONS[4][28] = {"Replay", "Edit custom move & replay", "Change gamemode", "Exit game"};
	char **moves=move_name_creation(p_usr_in);
	struct move_4_combinations m4_com;
	struct round_status rnd_stat={0, 0, 1};
	struct round_results rnd_res={0, 0, 0, "n/a"};
	struct round_results *p_rnd_res=&rnd_res;
	p_rnd_res->p1_wins=p_rnd_res->p2_wins=0;
	move_4_combination_creation(&m4_com, p_usr_in);

	while (1) {
		while (p_rnd_res->p1_wins!=3 && p_rnd_res->p2_wins!=3) {
			printf("\n----ROUND %d----\n", rnd_stat.round_num);
			printf("[PLAYER'S TURN]\n");
			printf("Choose a move by its number:\n");
			for (int iteration=0; iteration<4; iteration++) {
				printf("%d. %s.\n", iteration+1, moves[iteration]);
			}
			int_input_prompt(p_usr_in, 1, 4);
			rnd_stat.p1_move=p_usr_in->int_in;
			rnd_stat.p1_move--;
			printf("[PLAYER HAS CHOSEN A MOVE]\n");
			srand(time(NULL));
			rnd_stat.p2_move=rand()%4;
			tmp_sleep(250000);
			printf("\n[BOT HAS CHOSEN A MOVE]\n");
			check_round_winner(&rnd_stat, &m4_com, &p_rnd_res);
			if (strcmp(p_rnd_res->winner, "NOBODY")!=0) {
				p_rnd_res->winner=(strcmp(p_rnd_res->winner, "P1")==0) ? "PLAYER" : "BOT";
			}
			tmp_sleep(250000);
			printf("\n[ROUND RESULTS]\n");
			printf("Winner: %s.\n", p_rnd_res->winner);
			printf("Player chose %s and bot chose %s.\n", \
					moves[rnd_stat.p1_move], moves[rnd_stat.p2_move]);
			printf("Player has won %d times and bot has won %d times.\n", \
					p_rnd_res->p1_wins, p_rnd_res->p2_wins);
			// Gradual increase as you get used to the results menu.
			if (rnd_stat.round_num<2 && p_rnd_res->replay_chosen==0) {
				tmp_sleep(3000000);
			} else if (rnd_stat.round_num==2 && p_rnd_res->replay_chosen==0) {
				tmp_sleep(2500000);
			} else {
				tmp_sleep(2000000);
			}
			rnd_stat.round_num++;
		}
		(p_rnd_res->p1_wins==3) ? printf("Congrats, you won!\n") : \
				 printf("You lost, better luck next time!\n");
		printf("\n[END-OF-GAME MENU]\n");
		for (int iteration=0; iteration<4; iteration++) {
			printf("%d. %s.\n", iteration+1, MENU_OPTIONS[iteration]);
		}
		int_input_prompt(p_usr_in, 1, 4);
		switch (p_usr_in->int_in) {
			case 1:
				p_rnd_res->p1_wins=p_rnd_res->p2_wins=0;
				p_rnd_res->replay_chosen=rnd_stat.round_num=1;
				continue;
			case 3:
				free_str_arr(moves, 4);
				return 0;
			case 4:
				free_str_arr(moves, 4);
				return 1;
			default:
				p_rnd_res->p1_wins=p_rnd_res->p2_wins=rnd_stat.round_num=0;
				move_name_realloc(moves, p_usr_in);
				move_4_combination_creation(&m4_com, p_usr_in);
				p_rnd_res->replay_chosen=1;
				continue;
		}
	}
}