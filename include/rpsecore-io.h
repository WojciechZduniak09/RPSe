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

#ifndef RPSECORE_IO_H
#define RPSECORE_IO_H

#include <stdbool.h>

typedef struct
{
    union input
    {
        int int_input;
        char char_input;
        char *str_input;
    } input;
    int interval[2];
    unsigned short int buffer_size;
} user_input_data_t;

unsigned short int rpse_io_enterToContinue(void);
void * rpse_io_threadedEnterToContinue(void *arg);
void rpse_io_str(user_input_data_t *input_data, bool insert_tab_before_input);
void rpse_io_int(user_input_data_t *input_data, bool insert_tab_before_input, char *prompt);
void rpse_io_yn(user_input_data_t *input_data, bool insert_tab_before_input);

#endif