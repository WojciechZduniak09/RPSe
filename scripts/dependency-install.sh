#! /bin/bash
#
#
# RPSe, a rock paper scissors game for Linux systems.
#
# Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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

cat <<EOF >&1
RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.
RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with RPSe.
If not, see <https://www.gnu.org/licenses/>.
EOF

REQUIRED_DEPENDENCIES=("gcc" "make")
OPTIONAL_DEPENDENCIES=("valgrind" "gdb")
SUPPORTED_PACKAGE_MANAGERS=("apt" "dnf" "pacman")
INSTALLATION_MODE=0

# Checking if the system supports either APT, DNF or PACMAN.
for MANAGER in "${SUPPORTED_PACKAGE_MANAGERS[@]}"; do
	$MANAGER --version >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		case $MANAGER in
			"apt")
				REQUIRED_DEPENDENCIES+=" libssl-dev"
				UPDATE_PACKAGE_LIST="sudo apt update"
				UPGRADE_PACKAGES="sudo apt upgrade -y"
				INSTALLATION_PRESET="sudo apt install -y"
				;;
			"dnf")
				REQUIRED_DEPENDENCIES+=" openssl-devel"
				UPDATE_PACKAGE_LIST="sudo dnf check-update"
				UPGRADE_PACKAGES="sudo dnf upgrade -y"
				INSTALLATION_PRESET="sudo dnf install -y"
				;;
			"pacman")
				REQUIRED_DEPENDENCIES+=" openssl"
				UPDATE_PACKAGE_LIST="sudo pacman -Sy"
				UPGRADE_PACKAGES="sudo pacman -Syu"
				INSTALLATION_PRESET="sudo pacman -Sy"
				;;
		esac
	fi

	if [ $? -ne 0 ] && [ $ITERATION -eq 2 ]; then
		echo -e "No compatible package manager detected. Refer to README.md for package lists."
		exit 1
	fi
done

# Requesting an installation mode.
while ! [[ $INSTALLATION_MODE =~ ^[1-4]$ ]]; do
	echo "<--- Installation options --->"
	echo "1. Install required dependencies."
	echo -e "\t(Used for compiling RPSe)."
	echo "2. Install optional dependencies."
	echo -e "\t(Used for debug.sh)."
	echo "3. Install all dependencies."
	echo "4. Cancel installation."
	read -p "Choose an option by number: " INSTALLATION_MODE
done

if [ $INSTALLATION_MODE -eq 4 ]; then
	exit 0
fi

# Upgrading packages.
$UPDATE_PACKAGE_LIST
$UPGRADE_PACKAGES

# Installing packages.
if [ $INSTALLATION_MODE -eq 1 ]; then
	for DEPENDENCY in "${REQUIRED_DEPENDENCIES[@]}"; do
		$INSTALLATION_PRESET $DEPENDENCY
	done
elif [ $INSTALLATION_MODE -eq 2 ]; then
	for DEPENDENCY in "${OPTIONAL_DEPENDENCIES[@]}"; do
		$INSTALLATION_PRESET $DEPENDENCY
	done
else
	for DEPENDENCY in "${REQUIRED_DEPENDENCIES[@]}"; do
		$INSTALLATION_PRESET $DEPENDENCY
	done

	for DEPENDENCY in "${OPTIONAL_DEPENDENCIES[@]}"; do
		$INSTALLATION_PRESET $DEPENDENCY
	done
fi

echo -e "\nDone!"