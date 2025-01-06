# RPSe development news #

## v0.3.0-alpha.3 ##
1. Added found players at the end of *rpse_broadcast_receiverLoop()*
2. Added *user type* macros in *rpsecore-broadcast.h* and added any other externally-used ones there, too.
3. Removed redundant code from *rpse_io_str()* and added *first_time_called* for safety.
4. Added thank you message at the end of the program.
5. Removed redundant reusable part in the receiver function.
6. Added mutexes to broadcast loop termination handler and broadcast publisher and receiver functions.
7. Replaced broadcast and recevier *do-while* loops with *while* loops.
8. Made broadcast interval be every 20 seconds for the sake of the network that'll be used.

# Older NEWS information #
Older news information can be found in the ```ONEWS.md``` archive file.

# Copyright and Licensing #

RPSe is released under license GNU General Public License v3.0

RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RPSe. If not, see <https://www.gnu.org/licenses/>.
