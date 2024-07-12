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

#include "../include/rpsecore-error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void rpse_error_blameDev(void) {
    fprintf(stderr, "Error detected, please blame the developer who made some dumb mistake\n");
    exit(1);
}

void rpse_error_checkStringMalloc(char* target_string) {
    if (target_string == NULL) {
        fprintf(stderr, "Error detected during memory allocation of a string.\n"
                "Possible plans of action, in order:\n"
                "\t1. Reload RPSe.\n"
                "\t2. Restart your system and reload RPSe.\n"
                "\t3. Notify an RPSe developer.\n");
        exit(1);
    }
}

void rpse_error_checkMoveDataMalloc(move_data_t *target_data) {
    if (target_data == NULL) {
        fprintf(stderr, "Error detected during memory allocation of move_data_t.\n"
                "Possible plans of action, in order:\n"
                "\t1. Reload RPSe.\n"
                "\t2. Restart your system and reload RPSe.\n"
                "\t3. Notify an RPSe developer.\n");
        exit(1);
    }
}

void rpse_error_checkStringArrayMalloc(char **target_str_arr) {
    if (target_str_arr == NULL) {
        fprintf(stderr, "Error detected during memory allocation of a string array.\n"
                "Possible plans of action, in order:\n"
                "\t1. Reload RPSe.\n"
                "\t2. Restart your system and reload RPSe.\n"
                "\t3. Notify an RPSe developer.\n");
        exit(1);
    }
}