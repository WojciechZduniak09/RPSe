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
#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-moveDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void 
rpse_error_errorMessage(const char* ACTION)
{
    const unsigned int DECORATOR_LEN = strlen(ACTION) + strlen("Error detected during .");
    for (unsigned int char_index = 0; char_index < DECORATOR_LEN; char_index++)
        fprintf(stderr, "/");
    fprintf(stderr, "\nError detected while %s.\n", ACTION);
    for (unsigned int char_index = 0; char_index < DECORATOR_LEN; char_index++)
        fprintf(stderr, "/");
    fprintf(stderr, "\nPossible plans of action, in order:\n"
                    "\t1. Reload RPSe.\n"
                    "\t2. Restart your system and reload RPSe.\n"
                    "\t3. Notify an RPSe developer.\n");
}


void
rpse_error_blameDev(void)
{
    perror("[WARNING] The dev was a bit stupid, dw about it. You'll get a segmentation fault soon... I mean it");
}