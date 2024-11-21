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

# To be run from RPSe/bin/
TARGET_FILE_PATH="../.broadcast_address.txt"

rm -rf "$TARGET_FILE_PATH"
touch "$TARGET_FILE_PATH"

ip addr show | grep brd | grep inet | awk '{print $4}' > "$BROADCAST_ADDRESS"
