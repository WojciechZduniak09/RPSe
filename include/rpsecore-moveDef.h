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

#ifndef RPSECORE_MOVEDEF_H
#define RPSECORE_MOVEDEF_H

#include "rpsecore-io.h"

typedef struct
{
	char **move_names;
	unsigned short int winning_combinations[6][2];
} move_data_t;

move_data_t* rpse_moveDef_setUpMoves(user_input_data_t *input_data);
unsigned short int rpse_moveDef_redoCustomMove(user_input_data_t *input_data, move_data_t *move_data);
unsigned short int rpse_moveDef_freeMoveData(move_data_t *move_data);

#endif
