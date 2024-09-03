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

#include "../include/rpsecore-io.h"
#include "../include/rpsecore-logic.h"
#include "../include/rpsecore-gamemode.h"
#include "../include/rpsecore-error.h"
#include <stdio.h>
#include <time.h>

static void rpse_main_licenseStatement(void) { \
	printf("RPSe is released under license GNU General Public License v3.0.\n" \
			"RPSe (Rock Paper Scissors expanded), a rock paper scissors game for Linux systems.\n\n" \
			"Copyright (C) 2024 Wojciech Zduniak, Marcin Zduniak\n\n" \
			"RPSe is free software: you can redistribute it and/or modify it under the terms of the " \
			"GNU General Public License as published\n" \
			"by the Free Software Foundation, either version 3 of the License, or (at your option) " \
			"any later version.\n" \
			"RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; " \
			"without even the implied warranty of\n" \
			"MERCHANTABILITY or FITNESS " \
			"FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n" \
			"You should have received a copy of the GNU General Public License along with RPSe." \
			"If not, see <https://www.gnu.org/licenses/>.\n\n"); \
}

int main(void) {
	printf("RPSe v0.1.0-alpha.1.\n");
	rpse_main_licenseStatement();
	rpse_io_enterToContinue();

	user_input_data_t *input_data = &(user_input_data_t) {
		.buffer_size = 0,
		.interval = {0, 0},
		.input.int_input = 0
	};

	struct timespec ts = {0, 0};

	while ((rpse_gamemode_menu(input_data) == 1) ? rpse_gamemode_pvp(&ts) : rpse_gamemode_pve(input_data, &ts) != 2);

	return 0;
}