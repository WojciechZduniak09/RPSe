#! /bin/bash
#
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

# Variable definitions.
GREEN_FG=`tput setaf 2`
RED_FG=`tput setaf 1`
RESET=`tput sgr0`

# Setting up RPSe executable.
echo "${RED_FG}This debug menu has been discontinued, a new one may be developed in the future${RESET}"
sleep 3.5
cd ../../
make clean
make all
if [ $? -ne 0  ]; then
	echo "${RED_FG}Compilation proccess has failed!${RESET}"
	echo "${RED_FG}Exit code 1 for debug.sh${RESET}"
	exit 1 
fi
cd bin/

# Requesting input.
while ! [[ $SETTING =~ ^[1-5]$ ]]; do
	echo "Here are the available test options:"
	echo "    1. Test with Valgrind."
	echo "    2. Test with GDB."
	echo "    3. Time execution."
	echo "    4. Run without testing tools."
	echo "    5. Do not run/test."
	read -p "Select your option's number here: " SETTING
done

# Testing.
if [ $SETTING -eq 1 ]; then
	echo "${GREEN_FG}Valgrind testing has been selected.${RESET}"
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./RPSe

elif [ $SETTING -eq 2 ]; then
	echo "${GREEN_FG}GDB testing has been selected.${RESET}"
	gdb ./RPSe

elif [ $SETTING -eq 3 ]; then
	echo "${GREEN_FG}Timing testing has been selected${RESET}"
	time ./RPSe

elif [ $SETTING -eq 4 ]; then
	echo "${GREEN_FG}Regular execution has been selected${RESET}"
	./RPSe

else
	echo "${GREEN_FG}No testing has been selected.${RESET}"
fi

# Cleanup.
cd ..
make clean
cd scripts/testing/