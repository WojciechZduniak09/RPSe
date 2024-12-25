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

#ifndef RPSECORE_GAMEMODEMENUS_H
#define RPSECORE_GAMEMODEMENUS_H

/*
==============
PLAYER NUMBERS
==============
(1) Host
(2) Guest
*/

#include "rpsecore-io.h"
#include "rpsecore-moveDef.h"
#include "rpsecore-roundCalc.h"
#include <stdbool.h>

typedef struct
{
    const char PLAYER_1_NAME[31];
    const char PLAYER_2_NAME[31];
    bool player_1_ready;
    bool player_2_ready;
} player_data_t;

unsigned short int rpse_sharedGamemodeMenus_roundSummary(round_info_t *round_info, move_data_t *move_data, \
                                                         player_data_t *player_data, const unsigned short int PLAYER_NUMBER);
short int rpse_sharedGamemodeMenus_endOfGameMenu(user_input_data_t *input_data, bool isForPvE);

void rpse_sharedGamemodeMenus_roundStartCountdown(void);

#endif