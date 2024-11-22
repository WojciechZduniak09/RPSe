# RPSe, a rock paper scissors game for Linux systems.
#
# Copyright (C) 2024 Wojciech Zduniak, Marcin Zduniak
#
# This file is part of RPSe.
#
# RPSe is free software: you can redistribute it and/or modify it under the terms of
# the GNU General Public License as published by the Free Software Foundation, 
# either version 3 of the License, or (at your option) any later version.
# RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with RPSe.
# If not, see <https://www.gnu.org/licenses/>.

COMPILER = gcc
CFLAGS = -Wall -g -Wextra -Werror

RED_FG=`tput setaf 1`
GREEN_FG=`tput setaf 2`
RESET=`tput sgr0`

all: dir_check bin/RPSe
.PHONY: clean

dir_check:
	@scripts/internal/dir_check.sh

bin/objects/rpsecore-broadcast.o: src/rpsecore-broadcast.c include/rpsecore-broadcast.h
	@echo "${GREEN_FG}Compiling rpsecore-broadcast...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-dll.o: src/rpsecore-dll.c include/rpsecore-dll.h
	@echo "${GREEN_FG}Compiling rpsecore-dll...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-error.o: src/rpsecore-error.c include/rpsecore-error.h
	@echo "${GREEN_FG}Compiling rpsecore-error...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-gamemode1.o: src/rpsecore-gamemode1.c include/rpsecore-gamemode1.h
	@echo "${GREEN_FG}Compiling rpsecore-gamemode1...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-gamemode2.o: src/rpsecore-gamemode2.c include/rpsecore-gamemode2.h
	@echo "${GREEN_FG}Compiling rpsecore-gamemode2...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-sharedGamemodeMenus.o: src/rpsecore-sharedGamemodeMenus.c include/rpsecore-sharedGamemodeMenus.h
	@echo "${GREEN_FG}Compiling rpsecore-sharedGamemodeMenus...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-io.o: src/rpsecore-io.c include/rpsecore-io.h
	@echo "${GREEN_FG}Compiling rpsecore-io...${RESET}"
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-moveDef.o: src/rpsecore-moveDef.c include/rpsecore-moveDef.h
	@echo "${GREEN_FG}Compiling rpsecore-moveDef...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/rpsecore-roundCalc.o: src/rpsecore-roundCalc.c include/rpsecore-roundCalc.h
	@echo "${GREEN_FG}Compiling rpsecore-roundCalc...${RESET}"	
	$(COMPILER) $(CFLAGS) -c $< -o $@

bin/objects/main.o: src/main.c include/rpsecore-io.h include/rpsecore-error.h \
					include/rpsecore-sharedGamemodeMenus.h include/rpsecore-gamemode1.h \
					include/rpsecore-gamemode2.h
	@echo "${GREEN_FG}Compiling main...${RESET}"
	$(COMPILER) $(CFLAGS) -c $< -o $@
	
bin/RPSe: bin/objects/main.o bin/objects/rpsecore-broadcast.o bin/objects/rpsecore-dll.o \
		  bin/objects/rpsecore-error.o bin/objects/rpsecore-gamemode1.o \
		  bin/objects/rpsecore-gamemode2.o bin/objects/rpsecore-sharedGamemodeMenus.o \
		  bin/objects/rpsecore-io.o bin/objects/rpsecore-moveDef.o \
		  bin/objects/rpsecore-roundCalc.o
	@echo "${GREEN_FG}Compiling RPSe...${RESET}"	
	$(COMPILER) $(CFLAGS) $^ -o $@
	@echo "${GREEN_FG}Compilation finished!${RESET}"
		
clean:
	@rm -rf bin/
	@echo "${GREEN_FG}bin/ directory removed!${RESET}"