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
#include <unistd.h>

/*
================
STATIC FUNCTIONS
================
*/

static void 
_rpse_error_errorMessage(const char* ACTION)
{
    fprintf(stderr, "Error detected during %s.\n"
                    "Possible plans of action, in order:\n"
                    "\t1. Reload RPSe.\n"
                    "\t2. Restart your system and reload RPSe.\n"
                    "\t3. Notify an RPSe developer.\n", ACTION);
}

void
rpse_error_blameDev(void)
{
    fprintf(stderr, "Error detected, please blame the developer who has made some dumb mistake!\n");
    exit(1);
}

/*
============================
DOUBLY-LINKED LIST FUNCTIONS
============================
*/

void
rpse_error_checkDLLNodePtrMalloc(dll_node_t **p_target_node)
{
    if (*p_target_node == NULL)
        {
        _rpse_error_errorMessage("allocation of a dll_node_t**");
        exit(1);
        }
}

void
rpse_error_checkDLLNodeMalloc(dll_node_t *target_node)
{
    if (target_node == NULL)
        {
        _rpse_error_errorMessage("allocation of a dll_node_t*");
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
        _rpse_error_errorMessage("allocation of a char*");
        exit(1);
        }
}

void
rpse_error_checkStringArrayMalloc(char **target_str_arr)
{
    if (target_str_arr == NULL)
        {
        _rpse_error_errorMessage("allocation of a char**");
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
        _rpse_error_errorMessage("allocation of a move_data_t*");
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
        _rpse_error_errorMessage("socket operation");
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
        _rpse_error_errorMessage("verification of a value to be less than 0");
}

/* This function assumes that fopen() was used earlier */

void
rpse_error_checkFileExistance(FILE *fptr)
{
    if (fptr == NULL)
        {
        _rpse_error_errorMessage("search for a file (does not exist)");
        fclose(fptr);
        exit(1);
        }
}