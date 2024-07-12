#! /bin/bash
#
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

COMPRESSION_MODE=0

# Requesting compression format.
while ! [[ $COMPRESSION_MODE =~ ^[1-3]$ ]]; do
	echo "<--- Compression options --->"
	echo "1. tar.gz."
	echo "2. tar.bz2."
	echo "3. tar.xz (recommended)."

	read -p "Choose an option by number: " COMPRESSION_MODE
done

# Setting up RPSe for compression.
cd ../../
TARGET_DIR=$PWD
cp -rf RPSe/ tmp/
rm -rf tmp/.git

# Compression.
case $COMPRESSION_MODE in
	1)
		tar -czf RPSe.tar.gz tmp/
		;;
	2)
		tar -cjf RPSe.tar.bz2 tmp/
		;;
	3)
		tar -cJf RPSe.tar.xz tmp/
		;;
esac

# Handling a compression failure
if [ $? -ne 0 ]; then
	rm -rf tmp/
	cd RPSe/scripts/
	echo -e "\nCompression failed!"
	exit 1
fi

# Cleanup.
rm -rf tmp/
cd RPSe/scripts
echo -e "\nCompression finished!"
echo -e "\nCompressed file directory: $TARGET_DIR"