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

#include "rpsecore-dll.h"
#include "rpsecore-moveDef.h"
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

void rpse_error_blameDev(void);

void rpse_error_checkDLLNodePtrMalloc(dll_node_t **p_target_node);
void rpse_error_checkDLLNodeMalloc(dll_node_t *target_node);

void rpse_error_checkStringMalloc(char *target_string);
void rpse_error_checkStringArrayMalloc(char **target_str_arr);

void rpse_error_checkMoveDataMalloc(move_data_t *target_move_data);

void rpse_error_checkSocketOpRetVal(const int RET_VAL, int *sockfd);

void rpse_error_checkThreadCreation(pthread_t thread);

void rpse_error_checkuShortMalloc(unsigned short int *target_ushort);

void rpse_error_checkLessThan0RetVal(const int RET_VAL);
void rpse_error_checkFileExistance(FILE *fptr);

#endif