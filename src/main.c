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

#include "../include/rpsecore-io.h"
#include "../include/rpsecore-sharedGamemodeMenus.h"
#include "../include/rpsecore-gamemode1.h"
#include "../include/rpsecore-gamemode2.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sodium.h>

#define PVP 1
#define EXIT_GAME 2

/*
================
STATIC FUNCTIONS
================
*/

static void
_rpse_main_licenseStatement(void) 
{
	printf("RPSe is released under license GNU General Public License v3.0.\n"
			"RPSe (Rock Paper Scissors expanded), a rock paper scissors game for Linux systems.\n\n"
			"Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak\n\n"
			"RPSe is free software: you can redistribute it and/or modify it under the terms of the "
			"GNU General Public License as published\n"
			"by the Free Software Foundation, either version 3 of the License, or (at your option) "
			"any later version.\n"
			"RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "
			"without even the implied warranty of\n"
			"MERCHANTABILITY or FITNESS "
			"FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n"
			"You should have received a copy of the GNU General Public License along with RPSe."
			"If not, see <https://www.gnu.org/licenses/>.\n\n");
}

static unsigned short int
_rpse_main_mainMenu(user_input_data_t *input_data)
{
	if (input_data == NULL)
		{
		perror("_rpse_main_mainMenu() --> input_data == NULL");
		exit(EXIT_FAILURE);
		}
    printf("<----- Main menu ----->\n"
            "1. Player vs Player (PvP - WIP).\n"
            "2. Player vs Bot (PvE).\n\n");

    input_data->interval[0] = 1;
    input_data->interval [1] = 2;
    
    if (rpse_io_int(input_data, false, "Select a gamemode by it's number: ") == EXIT_FAILURE)
		{
		perror("_rpse_main_mainMenu() --> rpse_io_int() == EXIT_FAILURE");
		exit(EXIT_FAILURE);
		}

    return input_data->input.int_input;
}

/*
=============
MAIN FUNCTION
=============
*/

int 
main(void)
{
	printf("RPSe v0.4.0-alpha.4.\n");
	_rpse_main_licenseStatement();
	rpse_io_enterToContinue();

	user_input_data_t *input_data =
	&(user_input_data_t)
	{
		.buffer_size = 0,
		.interval = {0, 0},
		.input.int_input = 0
	};
	unsigned short int selected_gamemode = 2; /* Just for initialisation */
	if (sodium_init() < 0)
		{
		perror("main() --> sodium_init() < 0");
		exit(EXIT_FAILURE);
		}
	do
		{
		selected_gamemode = _rpse_main_mainMenu(input_data);
		}
	while ((selected_gamemode == PVP) ? rpse_gamemode1_pvp(input_data) : rpse_gamemode2_pve(input_data) != EXIT_GAME);
	
	printf("Thank you for playing!!\n");

	return 0;
}
