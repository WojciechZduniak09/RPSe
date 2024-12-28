/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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
==========
USER TYPES
==========
(1) Server --> For finding match players
(2) Client --> For finding match hosts
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
(6) NONCE = unsigned char[24] --> Represents the encryption nonce

-------
Formats
-------
(1) For hosts looking for players (deemed as player 1).
    - USERNAME@RPSe.server/bindOn(IP)(PORT)/customMove(MOVENAME)(VALUES)/nonce=NONCE
(2) For clients looking for game hosts (deemed as player 2).
    - USERNAME@RPSe.client/invitesOn(IP)(PORT)/nonce=NONCE
*/

#include <stdbool.h>
#include <sodium.h>
#include "rpsecore-dll.h"

#define NONCE_SIZE crypto_secretbox_NONCEBYTES /* probably 24 */
typedef struct
{
    char nonce[NONCE_SIZE];
    char username[31];
    char message[126 + NONCE_SIZE + 16];
    char encrypted_message[126 + crypto_secretbox_MACBYTES + NONCE_SIZE]
} broadcast_data_t;

void rpse_broadcast_waitUntilInterval(void);
unsigned short int rpse_broadcast_verifyAndTrimDLLStructure(string_dll_node_t **head, const unsigned short int USER_TYPE, const char *USERNAME);
string_dll_node_t * rpse_broadcast_receiveBroadcast(void);
void * rpse_broadcast_broadcasterLoop(const broadcast_data_t *BROADCAST_DATA);
void * rpse_broadcast_receiverLoop(const unsigned short int USER_TYPE);

#endif
