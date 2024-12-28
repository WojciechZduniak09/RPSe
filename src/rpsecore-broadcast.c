/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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
#include <sodium.h>

#define BROADCAST_PORT 51673
#define RECEIVER_BUFFER_SIZE 101
#define RECEIVER_TIMEOUT 2.0 /* seconds */
#define MAX_BROADCASTS 15
#define BROADCAST_DURATION 2 /* seconds */
#define BROADCAST_INTERVAL 10 /* seconds */
#define SERVER_REGEX_CONSTANT "@RPSe\\.server\\/bindOn\\([0-9\\.]{7,15}\\)\\([0-9]{1,5}\\)"
#define CLIENT_REGEX_CONSTANT "@RPSe\\.client\\/invitesOn\\(0-9\\.){7,15}\\)\\([0-9]{1,5}\\)customMove\\(" \
                            "[a-zA-Z0-9]{1,30}\\)\\([tf]{3}\\)$"
#define MAX_BROADCAST_SIZE 118 /* bytes/chars */
#define BROADCAST_CHACHA20_ENCRYPTION_KEY "puTxV6ZLHgTSku61/e3C3hGp+chxUbrGs6+lxbBpraI=" /* It's constant as how else would users be able to know that one is a player or not? It's not like I own a central server or anything */
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
static unsigned short int
_rpse_broadcast_getBroadcastAddress(char *broadcast_addr_str)
{
    struct ifaddrs *ifaddr;
    int ret_val = getifaddrs(&ifaddr);
    if (ret_val < 0)
        {
        perror("\"ifaddr < 0\" while attempting to get available ifaddresses");
        return EXIT_FAILURE;
        }

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
            continue;
        else
            broadcast_addr_found = true;
        }

    if (!broadcast_addr_found)
        {
        rpse_error_errorMessage("broadcast address search");
        return EXIT_FAILURE;
        }
    
    freeifaddrs(ifaddr);
    ifaddr = NULL;

    return EXIT_SUCCESS;
}

/*
===============
INTERVAL WAITER
===============
*/

void
rpse_broadcast_waitUntilInterval(void)
{
    printf("Please wait...\n");
    time_t now;
    struct tm *current_time;
    unsigned short int seconds = 0;

    do
        {
        sleep(0.5);
        time(&now);
        current_time = localtime(&now);
        seconds = current_time->tm_sec;
        }
    while (seconds % BROADCAST_INTERVAL != 0);
}

/*
=====================
MORE STATIC FUNCTIONS
=====================
*/

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

/* Go down to around line 254 for the receiver */

static unsigned short int
_rpse_broadcast_doublePublishBroadcast(broadcast_data_t *broadcast_data)
{
    struct sockaddr_in broadcast_addr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        {
        perror("\"sockfd < 0\" after attempting to create it");
        rpse_error_errorMessage("attempting to create a UDP socket");
        return EXIT_FAILURE;
        }

    int reuse_addr_option = 1;
    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_option, sizeof(reuse_addr_option));
    if (ret_val < 0)
        {
        perror("\"ret_val < 0\" after attempting to make sockfd reusable");
        rpse_error_errorMessage("attempting to modify a UDP socket");
        return EXIT_FAILURE;
        };

    ret_val = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST, sizeof(ENABLE_BROADCAST));
    if (ret_val < 0)
        {
        perror("\"ret_val < 0\" after attempting to make sockfd broadcastable");
        rpse_error_errorMessage("attempting to modify a UDP socket");
        return EXIT_FAILURE;
        };

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(BROADCAST_PORT);
    char *broadcast_address = calloc(1, INET_ADDRSTRLEN);
    if (_rpse_broadcast_getBroadcastAddress(broadcast_address) == EXIT_FAILURE)
        {
        perror("Unable to get broadcast address");
        return EXIT_FAILURE;
        }
    broadcast_addr.sin_addr.s_addr = inet_addr(broadcast_address);

    /* We add the nonce to the end of the broadcast */
    randombytes_buf(broadcast_data->nonce, sizeof(broadcast_data->nonce));
    char ciphertext[sizeof(broadcast_data->encrypted_message)];
    if (crypto_stream_chacha20_xor(ciphertext, broadcast_data->message, strlen(broadcast_data->message), broadcast_data->nonce,
                                                               BROADCAST_CHACHA20_ENCRYPTION_KEY) != EXIT_SUCCESS)
    	{
	perror("Unable to encrypt broadcast");
	rpse_error_errorMessage("encrypting a UDP broadcast");
	return EXIT_FAILURE;
	}
    memset(broadcast_data->encrypted_message, strlen(broadcast_data->encrypted_message), 0);
    strncpy(broadcast_data->encrypted_message, ciphertext, strlen(ciphertext) + 1);
    strncpy(broadcast_data->encrypted_message, "/nonce=", strlen("/nonce=") + 1);
    strncpy(broadcast_data->encrypted_message, broadcast_data->nonce, strlen(broadcast_data->nonce) + 1);

    /* Done twice in case the first one fails */
    for (unsigned short int iteration = 0; iteration < 2; iteration++)
        ret_val = sendto(sockfd, broadcast_data->encrypted_message, strlen(*(broadcast_data->encrypted_message)), 0,
                         (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    free(broadcast_address);
    broadcast_address = NULL;

    return EXIT_SUCCESS;
}

/* 
==================
BROADCAST VERIFIER
==================

View header file for user types
*/

unsigned short int
rpse_broadcast_verifyAndTrimDLLStructure(string_dll_node_t **head, const unsigned short int P2P_TYPE, const char *USERNAME)
{
    if (head == NULL)
        {
        perror("\"head == NULL\" while attempting to verify nodes in a string DLL");
		rpse_error_blameDev();
        return EXIT_FAILURE;
        }

    if ((*head) == NULL)
        {
        perror("\"(*head) == NULL\" while attempting to verify nodes in a string DLL");
		rpse_error_blameDev();
        return EXIT_FAILURE;
        }

    regex_t compiled_regex;

    char *expected_pattern = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && expected_pattern == NULL; attempt++)
        expected_pattern = calloc(strlen(CLIENT_REGEX_CONSTANT) + 1, sizeof(char));
    
    if (expected_pattern == NULL)
        {
        perror("\"expected_pattern == NULL\" while attempting to calloc() memory for it.");
        rpse_error_errorMessage("attempting to calloc() memory for a string");
        return EXIT_FAILURE;
        }

    strncpy(expected_pattern, "", strlen("") + 1);
    int ret_val;

    if (USERNAME == NULL)
        strncat(expected_pattern, "^[a-zA-Z0-9]{1,30}", strlen("^[a-zA-Z0-9]{1,30}") + 1);
    else
        strncat(expected_pattern, USERNAME, strlen(USERNAME) + 1);
    
    switch (P2P_TYPE)
        {
        case 1:
            strncat(expected_pattern, SERVER_REGEX_CONSTANT, strlen(SERVER_REGEX_CONSTANT) + 1);
            break;
        case 2:
            strncat(expected_pattern, CLIENT_REGEX_CONSTANT, strlen(CLIENT_REGEX_CONSTANT) + 1);
            break;        
        }

    ret_val = regcomp(&compiled_regex, expected_pattern, 0);

    if (ret_val != EXIT_SUCCESS) 
        {
        rpse_error_blameDev();
        return EXIT_FAILURE;
        }

    string_dll_node_t *tmp_next;
    string_dll_node_t *tmp_current_node = (*head);

    const unsigned int TOTAL_INITIAL_NODES = rpse_dll_getStringDLLNodeCount(&tmp_current_node);

    for (
        unsigned int position = 1;
        position < TOTAL_INITIAL_NODES && tmp_current_node != NULL;
        position++
        )
        {
        if (tmp_current_node->next == NULL)
            tmp_next = NULL;
        else
            tmp_next = tmp_current_node->next;
        
        ret_val = regexec(&compiled_regex, tmp_current_node->data, 0, NULL, 0);
        if (ret_val != EXIT_SUCCESS)
            {
            if (rpse_dll_deleteAtDLLStringPosition(head, position) == EXIT_FAILURE) 
                {
                perror("Error while attempting to delete a string DLL node");
                rpse_error_errorMessage("attempting to delete a string DLL node");
                return EXIT_FAILURE;
                }
            position--; /* We are now technically at the same node number */
            }

        if (tmp_next == NULL)
                tmp_current_node = NULL;
        else
            tmp_current_node = tmp_next;   
        }

    tmp_next = NULL;
    tmp_current_node = NULL;

    free(expected_pattern);
    expected_pattern = NULL;

    regfree(&compiled_regex);

    return EXIT_SUCCESS;
}

/*
=================
RECEIVER FUNCTION
=================

The broadcast function isn't in this section as it is not needed outside of this file 
*/

string_dll_node_t *
rpse_broadcast_receiveBroadcast(void)
{
    struct sockaddr_in broadcaster_addr;

    int sockfd = -1;
    for (unsigned short int attempt = 0; attempt < 3 && sockfd < 0; attempt++)
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        
    if (sockfd < 0)
        {
        perror("\"sockfd < 0\" after attempting to create it");
        rpse_error_errorMessage("attempting to create a UDP socket");
        return NULL;
        }

    int reuse_addr_option = 1;
    int ret_val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_option, sizeof(reuse_addr_option));
    if (ret_val < 0)
        {
        perror("\"ret_val < 0\" after attempting to make sockfd reusable");
        rpse_error_errorMessage("attempting to modify a UDP socket");
        return NULL;
        };
    
    struct timeval socket_timeout;
    socket_timeout.tv_sec = 2; /* receiver timeout in seconds */
    socket_timeout.tv_usec = 0;

    ret_val = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout));
    if (sockfd < 0)
        {
        perror("\"ret_val < 0\" after make sockfd timeout");
        rpse_error_errorMessage("attempting to modify a UDP socket");
        return NULL;
        }

    memset(&broadcaster_addr, 0, sizeof(broadcaster_addr));
    broadcaster_addr.sin_family = AF_INET;
    broadcaster_addr.sin_port = htons(BROADCAST_PORT);

    char *broadcast_address = NULL;

    for (unsigned short int attempt = 0; attempt < 3 && broadcast_address == NULL; attempt++)
        broadcast_address = calloc(INET_ADDRSTRLEN, sizeof(char));
    
    if (broadcast_address == NULL)
        {
        perror("\"broadcast_address == NULL\" while attempting to calloc() memory fot it");
        rpse_error_errorMessage("attempting to calloc() memory for a string");
        return NULL;
        }
    
    _rpse_broadcast_getBroadcastAddress(broadcast_address);
    broadcaster_addr.sin_addr.s_addr = inet_addr(broadcast_address);
    socklen_t receiver_sock_len = sizeof(broadcaster_addr);

    ret_val = bind(sockfd, (struct sockaddr *)&broadcaster_addr, sizeof(broadcaster_addr));
    if (ret_val < 0)
        {
        perror("Unable to bind UDP socket");
        return NULL;
        }

    string_dll_node_t* head = rpse_dll_createStringDLL("");
    time_t start = time(NULL);

    char *current_buffer = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && current_buffer == NULL; attempt++)
        current_buffer = calloc(RECEIVER_BUFFER_SIZE, sizeof(char));

    if (current_buffer == NULL)
        {
        perror("\"current_buffer == NULL\" while attempting to calloc() memory fot it");
        rpse_error_errorMessage("attempting to calloc() memory for a string");
        return NULL;
        }

    printf("Searching for players on your network, please wait.\n");

    while (difftime(time(NULL), start) < RECEIVER_TIMEOUT)
        {
        
        int received_broadcast_len = recvfrom(sockfd, current_buffer, RECEIVER_BUFFER_SIZE, 0,
                                              (struct sockaddr *)&broadcaster_addr, &receiver_sock_len);
        if (received_broadcast_len < 0)
            continue;
        
        if (strlen(current_buffer) > 1)
            current_buffer[received_broadcast_len] = '\0';

        if (head->data == NULL)
            head->data = current_buffer;
        else
            rpse_dll_insertAtStringDLLEnd(&head, current_buffer);

        memset(current_buffer, 0, RECEIVER_BUFFER_SIZE);
        }

    /* We get rid of the nonce from each of the messages and decrypt them */
    string_dll_node_t *current_node = head;
    do
        {
        broadcast_data_t *current_broadcast_data;
        memcpy(current_broadcast_data->encrypted_message, current_node->data, 125 + crypto_secretbox_MACBYTES);
        memcpy(current_broadcast_data->nonce, current_node->data + 126 + crypto_secretbox_MACBYTES, NONCE_SIZE); /* NONCE_SIZE is from header */
        memset(current_node->data, 0, strlen(current_node->data) + 1);
        crypto_stream_chacha20_xor(current_node->data, current_broadcast_data->encrypted_message, strlen(current_broadcast_data->encrypted_message) + 1,
                                                             current_broadcast_data->nonce, BROADCAST_CHACHA20_ENCRYPTION_KEY);
        if (current_node->next == NULL)
            current_node = NULL;
        else
            current_node = current_node->next;
        }
    while (current_node != NULL);

    free(current_buffer);
    current_buffer = NULL;

    free(broadcast_address);
    broadcast_address = NULL;

    rpse_dll_deleteStringDLL(&head);

    close(sockfd);
    return head;
}

/*
===================================
BROADCASTER/RECEIVER LOOP FUNCTIONS
===================================
*/

/* Should be threaded, view header for user types */
void *
rpse_broadcast_broadcasterLoop(const broadcast_data_t *BROADCAST_DATA)
{
    signal(SIGUSR1, _rpse_broadcast_handleTerminationSignal);

    do
        {
        rpse_broadcast_waitUntilInterval();
        _rpse_broadcast_doublePublishBroadcast(BROADCAST_DATA);
        }
    while (broadcaster_termination_flag == 0);

    signal(SIGUSR1, SIG_DFL);

    pthread_exit(NULL);
    return NULL;
}

/* Should be threaded, view header for user types */
void *
rpse_broadcast_receiverLoop(const unsigned short int P2P_TYPE)
{
    signal(SIGUSR2, _rpse_broadcast_handleTerminationSignal);

    do
        {
        rpse_broadcast_waitUntilInterval();

        string_dll_node_t *head = rpse_broadcast_receiveBroadcast();

        rpse_dll_deleteStringDLLDuplicateNodes(&head);
        rpse_broadcast_verifyAndTrimDLLStructure(&head, P2P_TYPE, NULL);

        rpse_dll_deleteStringDLL(&head);

        /* ADD MENU HERE */
        }
    while (receiver_termination_flag == 0);

    signal(SIGUSR2, SIG_DFL);

    pthread_exit(NULL);
    return NULL;
}
