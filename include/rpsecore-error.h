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

#ifndef RPSECORE_ERROR_H
#define RPSECORE_ERROR_H

#include <stdbool.h>
#include "rpsecore-logic.h"

void rpse_error_blameDev(void);
void rpse_error_checkStringMalloc(char* target_string);
void rpse_error_checkMoveDataMalloc(move_data_t *target_data);
void rpse_error_checkStringArrayMalloc(char **target_str_arr);

#endif