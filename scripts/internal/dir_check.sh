#! /bin/bash
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

cat <<EOF >&2
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

# To be run from RPSe/
check_directory() {
	if [ ! -d "$1" ]; then
		echo "Directory $1/ not found!"
		mkdir "$1"
		echo "Directory $1/ has been created!"
	fi
}

check_directory "bin"
check_directory "bin/objects"