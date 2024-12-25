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

#ifndef RPSECORE_ROUNDCALC_H
#define RPSECORE_ROUNDCALC_H

#include "rpsecore-moveDef.h"
#include <stdbool.h>

typedef struct
{
	char winner[7];
	unsigned short int round_num;
	unsigned short int p1_wins;
	unsigned short int p1_move;
	unsigned short int p2_wins;
	unsigned short int p2_move;
	bool replay;
} round_info_t;

unsigned short int rpse_roundCalc_getWinner(round_info_t *round_info, move_data_t *move_data);
unsigned short int rpse_roundCalc_prepNewMatch(round_info_t *round_info);

#endif