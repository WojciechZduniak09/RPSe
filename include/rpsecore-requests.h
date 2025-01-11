/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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

-------
Formats
-------
(1) For hosts looking for players (deemed as player 1).
	+ Decrypted message:
		- USERNAME@RPSe.server/requestBindOn(IP)(PORT)
	+ Encrypted message:
		- gibberish/nonce=NONCE
(2) For clients looking for game hosts (deemed as player 2).
	+ Decrypted message:
		- USERNAME@RPSe.client/requestJoinOn(IP)(PORT)
	+ Encrypted message:
		- gibberish/nonce=NONCE
*/

#ifndef RPSECORE_REQUESTS_H
#define RPSECORE_REQUESTS_H

#endif
