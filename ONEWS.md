# RPSe development news archive #

## v0.3.0-alpha.3 ##
1. Added found players at the end of *rpse_broadcast_receiverLoop()*
2. Added *user type* macros in *rpsecore-broadcast.h* and added any other externally-used ones there, too.
3. Removed redundant code from *rpse_io_str()* and added *first_time_called* for safety.
4. Added thank you message at the end of the program.
5. Removed redundant reusable part in the receiver function.
6. Added mutexes to broadcast loop termination handler.
7. Replaced broadcast and recevier *do-while* loops with *while* loops.
8. Made broadcast interval be every 20 seconds for the sake of the network that'll be used.
9. Changed receiver port to a dedicated one to avoid conflicts.
10. Made the broadcast address get function not return the loopback broadcast address *facepalm*.

## v0.2.0-alpha.2 ##
1. Added contact information for Wojciech Zduniak.
2. Added GPLv3 logo to *README.md*.
3. Added *libsodium* as dependency in *README.md* and *dependency-install.sh*.
4. Fixed *dependency-install.sh*.
5. Added **ChaCha20 encryption** to *rpsecore-broadcast-h* and *rpsecore-broadcast-c*.
6. Removed redundant DLL deletion at the end of *rpsecore_broadcast_broadcasterLoop()*.

## v0.1.0-alpha.1 ##
1. ```rpsecore-logic``` divided into ```rpsecore-dll```, ```rpsecore-moveDef```, ```rpsecore-sharedGamemodeMenus``` and ```rpsecore-roundCalc.c```.
2. Fully overhauled ```rpsecore-io``` functions.
3. Added cooldown for round result menu.
4. Removed most ```rpse-error``` functions in favour of more function-specific error handling.
5. Created ```rpsecore-broadcast``` for UDP broadcasts (soon in v0.2.0-alpha.1).
6. Made gamemode menus from ```rpsecore-logic``` static in the functions that only use them (```main.c``` and ```rpsecore-gamemode2.c```).
7. Created ```NEWS.md``` and ```ONEWS.md```.
8. Project renamed to ```Rock Paper Scissors extended```.
9. Adapted ```Makefile``` to source code changes.
10. Clarified red text in ```debug.sh```.
11. Adapted C code structure to follow GNU standards.
12. Adapted ```.gitignore``` to not have release nor TODO files in it.
13. Fixed ```rpsecore-moveDef```, ```rpsecore-broadcast```, ```rpsecore-gamemode1``` and ```rpsecore-dll``` memory failures.
14. Added mutex to ```rpsecore-io.c``` for multithreaded function ```rpse_io_threadedEnterToContinue(void *arg()```.
15. Removed ```exit(1)``` from ```rpse_broadcast_receiveBroadcast()``` and replaced it with a ```continue```.
16. Made ```_rpse_broadcast_getBroadcastAddress()``` more GNU-standard-compliant.
17. Added memset() for input string in ```_rpse_gamemode1_usernameMenu()```.
18. Removed buffer size in gamemode menu.
19. Added user type menu.
20. Made username menu return a status value.
21. Removed obsolete directories.
22. Added username validity confirmation to username creation menu.
23. Removed ```obsolete/``` directory.
24. Added parentheses to installation script's additional info for options 1 and 2.
25. Fixed ```rpse_broadcast_waitUntilInterval()``` and made it an non-static function.
26. Added GPL3 info to ```ONEWS.md``` file.

# Copyright and Licensing #

RPSe is released under license GNU General Public License v3.0

RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RPSe. If not, see <https://www.gnu.org/licenses/>.
