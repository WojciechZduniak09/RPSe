# RPSe development news #

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
# Older NEWS data #
Older news data can be found in the ```ONEWS.md``` file.

# Copyright and Licensing #

RPSe is released under license GNU General Public License v3.0

RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024 Wojciech Zduniak, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RPSe. If not, see <https://www.gnu.org/licenses/>.
