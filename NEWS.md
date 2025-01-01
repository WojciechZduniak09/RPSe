# RPSe development news #

## v0.2.0-alpha.2 ##
1. Added contact information for Wojciech Zduniak.
2. Added GPLv3 logo to *README.md*.
3. Added *libsodium* as dependency in *README.md* and *dependency-install.sh*.
4. Fixed *dependency-install.sh*.
5. Added **ChaCha20 encryption** to *rpsecore-broadcast-h* and *rpsecore-broadcast-c*.
6. Removed redundant DLL deletion at the end of *rpsecore_broadcast_broadcasterLoop()*.
7. Added found players at the end of *rpse_broadcast_receiverLoop()*
8. Added *user type* macros in *rpsecore-broadcast.h* and added any other externally-used ones there, too.
9. Removed redundant code from *rpse_io_str()* and added *first_time_called* for safety.
10. Added thank you message at the end of the program.
11. Removed redundant reusable part in the receiver function.
12. Added mutexes to broadcast loop termination handler.
13. Replaced broadcast and recevier *do-while* loops with *while* loops.
14. Made broadcast interval be every 20 seconds for the sake of the network that'll be used.

# Older NEWS information #
Older news information can be found in the ```ONEWS.md``` archive file.

# Copyright and Licensing #

RPSe is released under license GNU General Public License v3.0

RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RPSe. If not, see <https://www.gnu.org/licenses/>.
