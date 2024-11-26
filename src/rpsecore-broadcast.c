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
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <string.h>
#include <regex.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#define BROADCAST_PORT 51673
#define RECEIVER_BUFFER_SIZE 101
#define RECEIVER_TIMEOUT 2.0 /* seconds */
#define MAX_BROADCASTS 15
#define BROADCAST_DURATION 2 /* seconds */
#define BROADCAST_INTERVAL 10 /* seconds */
#define P2P1_REGEX_CONSTANT "@RPSe\\.P2P1\\/bindOn\\([0-9\\.]{7,15}\\)\\([0-9]{1,5}\\)\\/"
#define P2P2_REGEX_CONSTANT "@RPSe\\.P2P2\\/invitesOn\\(0-9\\.){7,15}\\)\\([0-9]{1,5}\\)customMove\\(" \
                      "[a-zA-Z0-9]{1,30}\\)\\([tf]{3}\\)$"
#define MAX_BROADCAST_SIZE 118 /* bytes/chars */

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

/* Parameter must be allocated to INET_ADDRSTRLEN */
static void
_rpse_broadcast_getBroadcastAddress(char *broadcast_addr_str)
{
    struct ifaddrs *ifaddr;
    rpse_error_checkLessThan0RetVal(getifaddrs(&ifaddr));

    struct ifaddrs *ifa;
    bool broadcast_addr_found = false;

    for (ifa = ifaddr; ifa != NULL && !broadcast_addr_found; ifa = ifa->ifa_next)
        {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        
        struct sockaddr_in broadcast_addr;
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_addr.s_addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr |
                                         ~((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr;
        
        const char *RET_VAL = inet_ntop(AF_INET, &broadcast_addr.sin_addr, broadcast_addr_str, INET_ADDRSTRLEN);
        if (RET_VAL == NULL)
            {
            continue;
            }
        else
            broadcast_addr_found = true;
        }

    if (!broadcast_addr_found)
        {
        rpse_error_errorMessage("broadcast address search");
        exit(1);
        }
    
    freeifaddrs(ifaddr);
    ifaddr = NULL;
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

    int reuse_addr_option = 1;
    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_option, sizeof(reuse_addr_option));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    ret_val = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST, sizeof(ENABLE_BROADCAST));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(BROADCAST_PORT);
    char *broadcast_address = calloc(1, INET_ADDRSTRLEN);
    _rpse_broadcast_getBroadcastAddress(broadcast_address);
    broadcast_addr.sin_addr.s_addr = inet_addr(broadcast_address);

    /* Done twice in case the first one fails */
    for (unsigned short int iteration = 0; iteration < 2; iteration++)
        {
        ret_val = sendto(sockfd, BROADCAST_DATA->message, strlen(*(BROADCAST_DATA->message)), 0,
            (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
        }

    free(broadcast_address);
    broadcast_address = NULL;
}

/* 
==================
BROADCAST VERIFIER
==================

View header file for P2P types
*/

bool
rpse_broadcast_verifyDLLStructure(dll_node_t **head, const unsigned short int P2P_TYPE, char *username)
{
    regex_t compiled_regex;
    char *expected_pattern = calloc(strlen(P2P2_REGEX_CONSTANT) + 1, sizeof(char));
    strncpy(expected_pattern, "", strlen("") + 1);
    int ret_val;

    if (username == NULL)
        strncat(expected_pattern, "^[a-zA-Z0-9]{1,30}", strlen("^[a-zA-Z0-9]{1,30}") + 1);
    else
        strncat(expected_pattern, username, strlen(username) + 1);
    
    switch (P2P_TYPE)
        {
        case 1:
            strncat(expected_pattern, P2P1_REGEX_CONSTANT, strlen(P2P1_REGEX_CONSTANT) + 1);
            break;
        case 2:
            strncat(expected_pattern, P2P2_REGEX_CONSTANT, strlen(P2P2_REGEX_CONSTANT) + 1);
            break;        
        }

    ret_val = regcomp(&compiled_regex, expected_pattern, 0);

    if (ret_val != 0)
        rpse_error_blameDev();
    
    dll_node_t *tmp = *head;

    bool dll_data_structure_is_valid = false;
    const unsigned int TOTAL_NODES = rpse_dll_getNodeCount(head);

    for (unsigned int position = 1; position < TOTAL_NODES && tmp->next != NULL; position++)
        {
        ret_val = regexec(&compiled_regex, tmp->data, 0, NULL, 0);

        if (ret_val != 0)
            rpse_dll_deleteAtDLLPostion(head, position);
            
        else
            dll_data_structure_is_valid = true;
        }
    
    free(tmp);
    tmp = NULL;

    free(expected_pattern);
    expected_pattern = NULL;

    return dll_data_structure_is_valid;
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
    struct sockaddr_in broadcaster_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    rpse_error_checkLessThan0RetVal(sockfd);

    int reuse_addr_option = 1;
    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_option, sizeof(reuse_addr_option));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);
    
    struct timeval socket_timeout;
    socket_timeout.tv_sec = 2; /* receiver timeout in seconds */
    socket_timeout.tv_usec = 0;
    ret_val = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    memset(&broadcaster_addr, 0, sizeof(broadcaster_addr));
    broadcaster_addr.sin_family = AF_INET;
    broadcaster_addr.sin_port = htons(BROADCAST_PORT);
    char *broadcast_address = malloc(INET_ADDRSTRLEN);
    broadcast_address = malloc(INET_ADDRSTRLEN);
    _rpse_broadcast_getBroadcastAddress(broadcast_address);
    broadcaster_addr.sin_addr.s_addr = inet_addr(broadcast_address);
    socklen_t receiver_sock_len = sizeof(broadcaster_addr);

    ret_val = bind(sockfd, (struct sockaddr *)&broadcaster_addr, sizeof(broadcaster_addr));
    rpse_error_checkSocketOpRetVal(ret_val, &sockfd);

    dll_node_t* head = rpse_dll_createDLL("");
    time_t start = time(NULL);

    char *buffer = calloc(RECEIVER_BUFFER_SIZE, sizeof(char) );
    rpse_error_checkStringMalloc(buffer);

    char *initial_buffer_val = calloc(RECEIVER_BUFFER_SIZE, sizeof(char));
    rpse_error_checkStringMalloc(initial_buffer_val);

    printf("Searching for players on your network, please wait.\n");

    while (difftime(time(NULL), start) < RECEIVER_TIMEOUT)
        {
        strncpy(initial_buffer_val, buffer, strlen(buffer) + 1);
        
        int received_broadcast_len = recvfrom(sockfd, buffer, RECEIVER_BUFFER_SIZE, 0,
                                              (struct sockaddr *)&broadcaster_addr, &receiver_sock_len);
        if (received_broadcast_len == -1 && strncmp(buffer, initial_buffer_val, RECEIVER_BUFFER_SIZE) != 0)
            {
            rpse_error_errorMessage("recvfrom()");
            close(sockfd);
            exit(1);
            }
        
        if(strlen(buffer) > 1)
            buffer[received_broadcast_len] = '\0';

        if (head->data == NULL)
            head->data = buffer;
        else
            rpse_dll_insertAtDLLEnd(&head, buffer);

        memset(buffer, 0, RECEIVER_BUFFER_SIZE);
        memset(initial_buffer_val, 0, RECEIVER_BUFFER_SIZE);
        }

    free(buffer);
    buffer = NULL;

    close(sockfd);
    return head;
}

/*
===================================
BROADCASTER/RECEIVER LOOP FUNCTIONS
===================================
*/

/* Should be threaded, view header for P2P types */
void *
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

    pthread_exit(NULL);
    return NULL;
}
/* Should be threaded, view header for P2P types */
void *
rpse_broadcast_receiverLoop(const unsigned short int P2P_TYPE)
{
    signal(SIGUSR2, _rpse_broadcast_handleTerminationSignal);

    do
        {
        _rpse_broadcast_waitUntilInterval();

        dll_node_t *head = rpse_broadcast_receiveBroadcast();

        rpse_dll_deleteDLLDuplicateNodes(&head);
        rpse_broadcast_verifyDLLStructure(&head, P2P_TYPE, NULL);

        rpse_dll_deleteDLL(&head);

        /* ADD MENU HERE */
        }
    while (receiver_termination_flag == 0);

    signal(SIGUSR2, SIG_DFL);

    pthread_exit(NULL);
    return NULL;
}