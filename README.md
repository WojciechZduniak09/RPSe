# RPSe (Rock Paper Scissors extended) - v0.1.0-alpha.1 - Index #
1. [Copyright information and licensing](#copyright-and-licensing).
2. [Purpose of this software](#purpose-of-this-software).
3. [Installation guide](#installation-guide).
4. [List of contrbutors](#list-of-contributors).

## Copyright and licensing ##
![RPSe uses the GNU General Public Licencse v3.0 and can be used with later versions of the GNU General Public License](images/gplv3-or-later.png "This project uses the GPL v2.0 or later versions, if desired")

RPSe is released under license GNU General Public License v3.0

RPSe, a rock paper scissors game for Linux systems.

Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

This file is part of RPSe.

RPSe is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with RPSe. If not, see <https://www.gnu.org/licenses/>.

## Purpose of this software ##
  * This is rock paper scissors but with an extra move to play with. Basically, you can play either rock, paper, scissors or x, which you can fully customise (by a name and custom properties).
  * PvE is only available at this time.
  * Current version: v0.1.0-alpha.1 (03/09/2024).
  * This software is being made for educational purposes to learn C lang.
  * Development schedules are very unstable, so updates will probably be spaced out by a long time.

## Installation guide ##
To install RPSe, you must:
  1. Download all dependencies for your Linux system
      * make (required).
      * gcc (required).
      * openssl (required).
      * gdb (optional, for testing).
      * valgrind (optional, for testing).

Note: if your system uses *APT*, *DNF* or *pacman* as it's package manager, an automated shell script at 
      ```RPSe/scripts/dependency-install.sh``` can auto-install dependencies by running the script as you would
      RPSe. It may not work for non-Debian-derivative systems, so use with caution.

  2. Install RPSe directly from it's [GitHub repository](https://github.com/WojciechZduniak09/RPSe)'s releases section.
  3. Run ```make all``` inside RPSe's root directory (```RPSe/```) using a terminal.
Now, you can just go to ```RPSe/bin/``` and run RPSe by typing ```./RPSe``` in your terminal.

Note: running ```make clean``` to uninstall RPSe removes the entire ```RPSe/bin/``` directory!

## List of contributors ##
1. WojciechZduniak09 (Wojciech Zduniak)
    * Documentation.
    * Code.
    * Contact at <githubinquiries.ladder140@passinbox.com> for anything you'd like.
2. mz7mz7mz7 (Marcin Zduniak)
    * Prior .gitignore file.