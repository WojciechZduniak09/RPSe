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

#ifndef RPSECORE_DLL_H
#define RPSECORE_DLL_H

struct string_dll
{
    struct string_dll *prev;
    char *data;
    struct string_dll *next;
};

typedef struct string_dll string_dll_node_t;

int rpse_dll_getStringDLLNodeCount(string_dll_node_t **head);
string_dll_node_t* rpse_dll_createStringDLL(const char *DATA);
unsigned short int rpse_dll_insertAtStringDLLEnd(string_dll_node_t **head, const char *DATA);
unsigned short int rpse_dll_deleteAtDLLStringPosition(string_dll_node_t **head, const unsigned int DELETION_POSITION);
unsigned short int rpse_dll_deleteStringDLLDuplicateNodes(string_dll_node_t **head);
unsigned short int rpse_dll_deleteStringDLL(string_dll_node_t **head);

#endif