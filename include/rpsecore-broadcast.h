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

#ifndef RPSECORE_BROADCAST_H
#define RPSECORE_BROADCAST_H

/*
=========
P2P TYPES
=========
(1) P2P1 --> For finding match players
(2) P2P2 --> For finding match hosts
*/

/*
==============
MESSAGE FORMAT
==============

-------------------------------------
Maximum values and names excluding \0
-------------------------------------
(1) USERNAME = char[30]
(2) IP = char[15] --> Represents IPv4 address
(3) PORT = char[5] --> Represents port number
(4) MOVENAME = char[30] --> Represents custom move name
(5) VALUES = char[3] --> Each is t (true) or f (false) as bools to say whether MOVENAME can beat rock, paper 
                         and scissors, respectively

-------
Formats
-------
(1) For hosts looking for players
    - USERNAME@RPSe.P2P1/bindOn(IP)(PORT)/customMove(MOVENAME)(VALUES)
(2) For clients looking for game hosts.
    - USERNAME@RPSe.P2P2/invitesOn(IP)(PORT)
*/

#include <stdbool.h>
#include "rpsecore-dll.h"

typedef struct
{
    char username[31];
    char* message[118]; /* 118 is the maximum, not constant value */
} broadcast_data_t;

bool rpse_broadcast_verifyDLLStructure(dll_node_t **head, const unsigned short int P2P_TYPE, char *username);
dll_node_t * rpse_broadcast_receiveBroadcast(void);
void rpse_broadcast_broadcasterLoop(const broadcast_data_t *BROADCAST_DATA);
void rpse_broadcast_receiverLoop(const unsigned short int P2P_TYPE);

#endif