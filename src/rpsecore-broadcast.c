/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024 Wojciech Zduniak, Marcin Zduniak
 *
 * This file is part of RPSe.
 *
 * RPSe is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * RPSe is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with RPSe.
 * If not, see <https://www.gnu.org/licenses/>.
 */

/*
Fast explanation
    - Broadcast
        - Wait until start of a 10-second interval (00:00,:00, 00:00:10, etc.)
        - Broadcast custum message
        - 8 sec cooldown
        - Broadcast until signal SIGUSR1
    - Receiver
        - Wait until start of a 10-second interval
        - Receive 15 messages max
        - Return messages
        - Clean duplicate messages
        - Do not use signals as data must be processed afterwards
        - Must be in a separate loop
*/



#include "../include/rpsecore-broadcast.h"
#include "../include/rpsecore-error.h"
#include "../include/rpsecore-dll.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <signal.h>

#define BROADCAST_PORT 51673
#define RECEIVER_BUFFER_SIZE 101
#define RECEIVER_TIMEOUT 2 /* seconds */
#define MAX_BROADCASTS 15
#define BROADCAST_DURATION 2 /* seconds */
#define BROADCAST_INTERVAL 10 /* seconds */
#define P2P1_REGEX_CONSTANT "@RPSe\\.P2P1\\/bindOn\\([0-9\\.]{7,15}\\)\\([0-9]{1,5}\\)\\/"
#define P2P2_REGEX_CONSTANT "@RPSe\\.P2P2\\/invitesOn\\(0-9\\.){7,15}\\)\\([0-9]{1,5}\\)customMove\\(" \
                      "[a-zA-Z0-9]{1,30}\\)\\([tf]{3}\\)$"

/*
================
GLOBAL VARIABLES
================
*/

const unsigned short int ENABLE_BROADCAST = 1;
volatile sig_atomic_t broadcaster_termination_flag = 0;
volatile sig_atomic_t receiver_termination_flag = 0;

/*
================
STATIC FUNCTIONS
================
*/

/* MAKE THIS PURE C */
static char * 
_rpse_broadcast_checkBroadcastAddress(void)
{
    rpse_error_checkLessThan0RetVal(system("bash ../scripts/internal/get_broadcast_addr.sh"));
    
    FILE *fptr;
    fptr = fopen("../.broadcast_address.txt", "r");
    rpse_error_checkFileExistance(fptr);

    /* MUST BE FREED AFTER EXECUTION */
    char *contents = malloc(16 * sizeof(char));
    rpse_error_checkStringMalloc(contents);
    
    fgets(contents, 16, fptr);

    fclose(fptr);
    return contents;
}

static void 
_rpse_broadcast_waitUntilInterval(void)
{
    time_t now;
    struct tm *current_time;
    unsigned short int seconds = 0;

    do
        {
        time(&now);
        current_time = localtime(&now);
        seconds = current_time->tm_sec;
        sleep(1);
        }
    while (seconds % BROADCAST_INTERVAL == 0);
}

static void 
_rpse_broadcast_handleTerminationSignal(const int SIGNAL)
{
    if (SIGNAL == SIGUSR1)
        broadcaster_termination_flag = 1;
    else if (SIGNAL == SIGUSR2)
        receiver_termination_flag = 1;
    else
        return;
}

static void 
_rpse_broadcast_doublePublishBroadcast(const broadcast_data_t *BROADCAST_DATA)
{
    struct sockaddr_in broadcast_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    rpse_error_checkLessThan0RetVal(sockfd);

    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST, sizeof(ENABLE_BROADCAST));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(BROADCAST_PORT);
    char* broadcast_address = _rpse_broadcast_checkBroadcastAddress();
    broadcast_addr.sin_addr.s_addr = inet_addr(broadcast_address);

    free(broadcast_address);
    broadcast_address = NULL;

    /* Done twice in case the first one fails */
    for (unsigned short int iteration = 0; iteration < 2; iteration++)
        {
        ret_val = sendto(sockfd, BROADCAST_DATA->message, strlen(*(BROADCAST_DATA->message)), 0,
            (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
        }
}

/* 
==================
BROADCAST VERIFIER
==================

View header file for P2P types */

bool
rpse_broadcast_verifyDLLStructure(dll_node_t **head, const unsigned short int P2P_TYPE, char *username)
{
    regex_t compiled_regex;
    char *expected_pattern = malloc(19);
    int compilation_failed;
    switch (P2P_TYPE)
        {
        case 1:
            expected_pattern = (username == NULL) ? strcat(P2P1_REGEX_CONSTANT, "^[a-zA-Z0-9]{1,30}") :
                                                            strcat(P2P1_REGEX_CONSTANT, username);
            compilation_failed = regcomp(&compiled_regex, expected_pattern, 0);
            if (compilation_failed)
                rpse_error_blameDev();
            break;
        case 2:
            expected_pattern = (username == NULL) ? strcat(P2P2_REGEX_CONSTANT, "^[a-zA-Z0-9]{1,30}"):
                                                                  strcat(P2P2_REGEX_CONSTANT, username);    
            compilation_failed = regcomp(&compiled_regex, expected_pattern, 0);
            if (compilation_failed)
                rpse_error_blameDev();
            break;        
        }
    
    dll_node_t *tmp = *head;

    bool exact_match_found = false;

    for (unsigned int position = 1; position < rpse_dll_getNodeCount(head) &&
         tmp->next != NULL; position++)
        {
        int match_failed = regexec(&compiled_regex, tmp->data, 0, NULL, 0);

        if (match_failed)
            rpse_dll_deleteAtDLLPostion(head, position);
        else
            exact_match_found = true;
        }
    
    free(tmp);
    tmp = NULL;

    free(expected_pattern);
    expected_pattern = NULL;

    return exact_match_found;
}

/*
=================
RECEIVER FUNCTION
=================

The broadcast function isn't in this section as it is not needed outside of this file 
*/

dll_node_t *
rpse_broadcast_receiveBroadcast(void)
{
    struct sockaddr server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    rpse_error_checkLessThan0RetVal(sockfd);

    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST, sizeof(ENABLE_BROADCAST));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sa_family = AF_INET;
    /* fix this with port and broadcast addr */

    ret_val = bind(sockfd, &server_addr, sizeof(server_addr));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);
    
    unsigned int received_broadcast_len;
    time_t start_time = time(NULL);
    dll_node_t* head = rpse_dll_createDLL(NULL);

    while (difftime(time(NULL), start_time) < RECEIVER_TIMEOUT)
        {
        char *buffer = malloc(sizeof(char) * RECEIVER_BUFFER_SIZE);
        rpse_error_checkStringMalloc(buffer);

        received_broadcast_len = recvfrom(sockfd, buffer, RECEIVER_BUFFER_SIZE - 1, 0,
                                          &client_addr, &addr_len);
        rpse_error_checkSocketOpRetVal(received_broadcast_len, &sockfd);
        buffer[received_broadcast_len] = '\0';

        if (head->data == NULL)
            head->data = buffer;
        else
            rpse_dll_insertAtDLLEnd(&head, buffer);
        }

    close(sockfd);
    return head;
}

/*
===================================
BROADCASTER/RECEIVER LOOP FUNCTIONS
===================================
*/

/* Must be threaded to exit, view header for P2P types */
void 
rpse_broadcast_broadcasterLoop(const broadcast_data_t *BROADCAST_DATA)
{
    signal(SIGUSR1, _rpse_broadcast_handleTerminationSignal);

    do
        {
        _rpse_broadcast_waitUntilInterval();
        _rpse_broadcast_doublePublishBroadcast(BROADCAST_DATA);
        }
    while (broadcaster_termination_flag == 0);

    signal(SIGUSR1, SIG_DFL);
}

/* Must be threaded to exit, view header for P2P types */
void
rpse_broadcast_receiverLoop(const unsigned short int P2P_TYPE)
{
    signal(SIGUSR2, _rpse_broadcast_handleTerminationSignal);

    do
        {
        _rpse_broadcast_waitUntilInterval();

        dll_node_t *head = rpse_broadcast_receiveBroadcast();

        rpse_dll_deleteDLLDuplicateNodes(&head);
        rpse_broadcast_verifyDLLStructure(&head, P2P_TYPE, NULL);

        /* ADD MENU HERE */
        }
    while (receiver_termination_flag == 0);

    signal(SIGUSR2, SIG_DFL);
}