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

struct dll
{
    struct dll *prev;
    char *data;
    struct dll *next;
};

typedef struct dll dll_node_t;

dll_node_t* rpse_dll_createDLL(const char *DATA);
void rpse_dll_insertAtDLLEnd(dll_node_t **head, const char *DATA);
void rpse_dll_deleteAtDLLPostion(dll_node_t **head, const unsigned int DELETION_POSITION);
void rpse_dll_deleteDLL(dll_node_t **head);
void rpse_dll_deleteDLLDuplicateNodes(dll_node_t **head);
unsigned int rpse_dll_getNodeCount(dll_node_t **head);

#endif