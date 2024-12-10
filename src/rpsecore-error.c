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

/*
================
MESSAGE FUNCTION
================
*/

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

/*
============================
DOUBLY-LINKED LIST FUNCTIONS
============================
*/

void
rpse_error_checkDLLNodePtrMalloc(string_dll_node_t **p_target_node)
{
    if (*p_target_node == NULL)
        {
        rpse_error_errorMessage("allocation of a string_dll_node_t**");
        exit(1);
        }
}

void
rpse_error_checkDLLNodeMalloc(string_dll_node_t *target_node)
{
    if (target_node == NULL)
        {
        rpse_error_errorMessage("allocation of a string_dll_node_t*");
        exit(1);
        }
}

/*
================
STRING FUNCTIONS
================
*/

void
rpse_error_checkStringMalloc(char *target_string)
{
    if (target_string == NULL)
        {
        rpse_error_errorMessage("allocation of a char*");
        exit(1);
        }
}

void
rpse_error_checkStringArrayMalloc(char **target_str_arr)
{
    if (target_str_arr == NULL)
        {
        rpse_error_errorMessage("allocation of a char**");
        exit(1);
        }
}

/*
===================
MOVE DATA FUNCTIONS
===================
*/

void
rpse_error_checkMoveDataMalloc(move_data_t *target_move_data)
{
    if (target_move_data == NULL)
        {
        rpse_error_errorMessage("allocation of a move_data_t*");
        exit(1);
        }
}

/*
================
SOCKET FUNCTIONS
================
*/

void
rpse_error_checkSocketOpRetVal(const int RET_VAL, int *sockfd)
{
    if (RET_VAL < 0)
        {
        close(*sockfd);
        rpse_error_errorMessage("socket operation");
        exit(1);
        }
}

/*
==================
THREADED FUNCTIONS
==================
*/

void
rpse_error_checkFirstThreadCreation(const int RET_VAL)
{
    if (RET_VAL != 0)
        {
        rpse_error_errorMessage("thread creation");

        exit(1);
        }
}

/*
============
INT FUNCTION
============

I can't believe that this part is actualy necessary
*/

void
rpse_error_checkuShortMalloc(unsigned short int *target_ushort)
{
    if (target_ushort == NULL)
        {
        rpse_error_errorMessage("allocation of a ushort");
        exit(1);
        }
}

/*
=======================
MISCELLANEOUS FUNCTIONS
=======================
*/

void
rpse_error_checkLessThan0RetVal(const int RET_VAL)
{
    if (RET_VAL < 0)
        rpse_error_errorMessage("verification of a value to be less than 0");
}

/* This function assumes that fopen() was used earlier */

void
rpse_error_checkFileExistance(FILE *fptr)
{
    if (fptr == NULL)
        {
        rpse_error_errorMessage("search for a file (does not exist)");
        fclose(fptr);
        exit(1);
        }
}