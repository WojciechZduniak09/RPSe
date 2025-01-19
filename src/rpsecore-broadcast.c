/*
 * RPSe, a rock paper scissors game for Linux systems.
 *
 * Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak
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

#include "../include/rpsecore-broadcast.h"
#include "../include/rpsecore-dll.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sodium.h>
#include <netdb.h>

#define RECEIVER_BUFFER_SIZE 300
#define RECEIVER_TIMEOUT 2.0 /* seconds */
#define MAX_BROADCASTS 15
#define BROADCAST_DURATION 2 /* seconds */
#define BROADCAST_INTERVAL 20 /* seconds */
#define SERVER_REGEX_CONSTANT "\\@RPSe\\.server\\/bindOn\\([0-9\\.]{7,15}\\)\\([0-9]{1,5}\\)\\/customMove\\([a-zA-Z0-9]{1,30}\\)\\([tf]{3}\\)$"
#define CLIENT_REGEX_CONSTANT "\\@RPSe\\.client\\/invitesOn\\([0-9\\.]{7,15}\\)\\([0-9]{1,5}\\)$"
#define MAX_BROADCAST_SIZE 118 /* bytes/chars */
#define BROADCAST_CHACHA20_ENCRYPTION_KEY "puTxV6ZLHgTSku61/e3C3hGp+chxUbrGs6+lxbBpraI=" /* It's constant as how else would users be able to know that one is a player or not? It's not like I own a central server or anything */

/*
=================
GLOBAL VARIABLES
=================
*/

const int ENABLE_BROADCAST_OPTION = 1;
const int REUSE_ADDR_OPTION = 1;
const int REUSE_PORT_OPTION = 1;
pthread_mutex_t clock_lock = PTHREAD_MUTEX_INITIALIZER;

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
    if (getifaddrs(&ifaddr) < 0)
        {
        perror("_rpse_broadcast_getBroadcastAddress() --> getifaddrs() < 0");
        return EXIT_FAILURE;
        }

    struct ifaddrs *ifa;
    bool broadcast_addr_found = false;

    for (ifa = ifaddr; ifa != NULL && !broadcast_addr_found; ifa = ifa->ifa_next)
        {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET || (strcmp(ifa->ifa_name, "lo") == 0))
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
	perror("_rpse_broadcast_getBroadcastAddress() --> !broadcast_addr_found");
        return EXIT_FAILURE;
        }
    
    freeifaddrs(ifaddr);
    ifaddr = NULL;

    return EXIT_SUCCESS;
}

/* View header for user types */
static unsigned short int
_rpse_broadcast_verifyAndTrimDLLStructure(string_dll_node_t **head, const unsigned short int USER_TYPE, const char *USERNAME)
{
    if (head == NULL)
        {
        perror("_rpse_broadcast_verifyAndTrimDLLStructure() --> head == NULL");
        return EXIT_FAILURE;
        }

    if ((*head) == NULL)
        {
        perror("_rpse_broadcast_verifyAndTrimDLLStructure() --> (*head) == NULL");
        return EXIT_FAILURE;
        }

    regex_t compiled_regex;

    char *expected_pattern = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && expected_pattern == NULL; attempt++)
        expected_pattern = calloc(strlen((USER_TYPE == CLIENT_USER_TYPE) ? CLIENT_REGEX_CONSTANT : SERVER_REGEX_CONSTANT) + strlen(USERNAME) + 1, sizeof(char));
    
    if (expected_pattern == NULL)
        {
        perror("_rpse_broadcast_verifyAndTrimDLLStructure() --> expected_pattern == NULL");
        return EXIT_FAILURE;
        }

    if (USERNAME == NULL)
        strncat(expected_pattern, "^[a-za-z0-9]{1,30}", strlen("^[a-za-z0-9]{1,30}") + 1);
    else
        strncat(expected_pattern, USERNAME, strlen(USERNAME) + 1);
    
    switch (USER_TYPE)
        {
        case SERVER_USER_TYPE:
            strncat(expected_pattern, SERVER_REGEX_CONSTANT, strlen(SERVER_REGEX_CONSTANT) + 1);
            break;
        case CLIENT_USER_TYPE:
            strncat(expected_pattern, CLIENT_REGEX_CONSTANT, strlen(CLIENT_REGEX_CONSTANT) + 1);
            break;        
        }
    if (regcomp(&compiled_regex, expected_pattern, REG_EXTENDED) != EXIT_SUCCESS) 
        return EXIT_FAILURE;

    string_dll_node_t *tmp_next;
    string_dll_node_t *tmp_current_node = (*head);

    const unsigned int total_initial_nodes = rpse_dll_getStringDLLNodeCount(&tmp_current_node);

    for (
        unsigned int position = 1;
        position <= total_initial_nodes && tmp_current_node != NULL;
        position++
        )
        {
        if (tmp_current_node->next == NULL)
            tmp_next = NULL;
        else
            tmp_next = tmp_current_node->next;
        
        if (regexec(&compiled_regex, tmp_current_node->data, 0, NULL, 0) != EXIT_SUCCESS && head != NULL)
            {
            if (rpse_dll_deleteAtDLLStringPosition(head, position) == EXIT_FAILURE) 
                {
                perror("_rpse_broadcast_verifyAndTrimDLLStructure() --> rpse_dll_deleteAtStringDLLPosition() == EXIT_FAILURE");
                return EXIT_FAILURE;
                }
            position--; /* we are now technically at the same node index */
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
IP ADDRESS FINDER
=================
*/

char *
rpse_broadcast_getIPAddress(void)
{
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1)
        {
        perror("rpse_broadcast_getIPAddress() --> getifaddrs() == -1");
        return NULL;
        }
    
    struct ifaddrs *ifa;
    int family;
    char *host = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && host == NULL; attempt++)
        host = calloc(NI_MAXHOST, sizeof(char));
    if (host == NULL)
        {
        perror("rpse_broadcast_getIPAddress() --> host == NULL");
        return NULL;
        }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET)
            {
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST))
                {
                perror("rpse_broadcast_getIPAddress() --> getnameinfo() == EXIT_FAILURE");
                return NULL;
                }
            }
        }
    
    freeifaddrs(ifaddr);
    return host;
}

/* Go down to around line 374 for the receiver */

/*
===========
BROADCASTER
===========
*/

unsigned short int
rpse_broadcast_publishBroadcast(broadcast_data_t *broadcast_data)
{
    struct sockaddr_in broadcast_addr;

    int sockfd = -1;
    for (unsigned short int attempt = 0; attempt < 3 && sockfd < 0; attempt++)
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        {
        perror("rpse_broadcast_publishBroadcast() --> socket() < 0");
        return EXIT_FAILURE;
        }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &REUSE_ADDR_OPTION, sizeof(REUSE_ADDR_OPTION)) < 0)
        {
        perror("rpse_broadcast_publishBroadcast() --> setsockopt(SO_REUSEADDR) < 0");
        return EXIT_FAILURE;
        }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &REUSE_PORT_OPTION, sizeof(REUSE_PORT_OPTION)) < 0)
        {
        perror("rpse_broadcast_publishBroadcast() --> setsockopt(SO_REUSEPORT) < 0");
        return EXIT_FAILURE;
        }
   
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST_OPTION, sizeof(ENABLE_BROADCAST_OPTION))< 0)
        {
        perror("rpse_broadcast_publishBroadcast() --> setsockopt(SO_BROADCAST)");
        return EXIT_FAILURE;
        }

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons((unsigned short int)BROADCAST_PORT);
    char *broadcast_address = calloc(1, INET_ADDRSTRLEN);
    if (_rpse_broadcast_getBroadcastAddress(broadcast_address) == EXIT_FAILURE)
        {
        perror("rpse_broadcast_publishBroadcast() --> _rpse_broadcast_getBroadcastAddress() == EXIT_FAILURE");
        return EXIT_FAILURE;
        }
    broadcast_addr.sin_addr.s_addr = inet_addr(broadcast_address);

    /* We add the nonce to the end of the broadcast */
    memset(broadcast_data->nonce, 0, sizeof(broadcast_data->nonce));

    unsigned char binary_nonce[NONCE_SIZE];
    memset(binary_nonce, 0, sizeof(binary_nonce));
    randombytes_buf(binary_nonce, sizeof(binary_nonce));
    sodium_bin2base64(broadcast_data->nonce, sizeof(broadcast_data->nonce), binary_nonce, sizeof(binary_nonce), sodium_base64_VARIANT_ORIGINAL);

    char ciphertext[sizeof(broadcast_data->encrypted_message)] = {0};
    if (crypto_stream_chacha20_xor((unsigned char *)ciphertext, (const unsigned char *)broadcast_data->message, strlen(broadcast_data->message), (const unsigned char *)broadcast_data->nonce,
                                                               (const unsigned char *)BROADCAST_CHACHA20_ENCRYPTION_KEY) != EXIT_SUCCESS)
    	{
	perror("rpse_broadcast_publishBroadcast() --> crypto_stream_chacha20_xor() != EXIT_SUCCESS");
	return EXIT_FAILURE;
	}

    memset(broadcast_data->encrypted_message, 0, sizeof(broadcast_data->encrypted_message));
    strcat(broadcast_data->encrypted_message, ciphertext);
    strcat(broadcast_data->encrypted_message, "/nonce=");
    strcat(broadcast_data->encrypted_message, broadcast_data->nonce);

    if (sendto(sockfd, broadcast_data->encrypted_message, strlen((broadcast_data->encrypted_message)), 0,
        (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0)
	{
	perror("rpse_broadcast_publishBroadcast() --> sendto() < 0");
	return EXIT_FAILURE;
	}

    free(broadcast_address);
    broadcast_address = NULL;
    close(sockfd);

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
    time_t start_time = time(NULL);
    time_t now;
    struct tm *current_time;
    unsigned short int seconds = 0;

    do
        {
	pthread_mutex_lock(&clock_lock);
        time(&now);
        current_time = localtime(&now);
        seconds = current_time->tm_sec;
	pthread_mutex_unlock(&clock_lock);

	if ((seconds < 15 && seconds > 0) || (seconds < 35 && seconds > 20) || (seconds < 55 && seconds > 40))
		sleep(4);
	else if ((seconds > 15 && seconds < 19) || (seconds > 35 && seconds < 39) || (seconds > 55 && seconds < 59))
		sleep(1);
	else if (seconds % BROADCAST_INTERVAL == EXIT_SUCCESS)
		break;
	else
		sleep(0.25);

	if (difftime(start_time, time(NULL)) > 40.0)
		{
		perror("rpse_broadcast_waitUntilInterval --> TIMEOUT");
		abort();
		}
        }
    while (seconds % BROADCAST_INTERVAL != 0);
}

/*
=================
RECEIVER FUNCTION
=================
*/

string_dll_node_t *
rpse_broadcast_receiveBroadcast(const broadcast_data_t *BROADCAST_DATA)
{
    if (BROADCAST_DATA == NULL)
	{
	perror("rpse_broadcast_receiveBroadcast() --> BROADCAST_DATA == NULL");
	return NULL;
	}
    else if (BROADCAST_DATA->user_type != SERVER_USER_TYPE && BROADCAST_DATA->user_type != CLIENT_USER_TYPE)
    	{
	perror("rpse_broadcast_receiveBroadcast() --> BROADCAST_DATA->user_type INVALID");
	return NULL;
        }
    
    struct sockaddr_in server_template_addr;

    int sockfd = -1;
    for (unsigned short int attempt = 0; attempt < 3 && sockfd < 0; attempt++)
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> socket()");
        return NULL;
        }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &REUSE_ADDR_OPTION, sizeof(REUSE_ADDR_OPTION)) < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> setsockopt(SO_REUSEADDR)");
        return NULL;
        }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &REUSE_PORT_OPTION, sizeof(REUSE_PORT_OPTION)) < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> setsockopt(SO_REUSEPORT)");
        return NULL;
        }

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &ENABLE_BROADCAST_OPTION, sizeof(ENABLE_BROADCAST_OPTION)) < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> setsockopt(SO_BROADCAST)");
        return NULL;
        }
        
    struct timeval socket_timeout;
    socket_timeout.tv_sec = 2; /* receiver timeout in seconds */
    socket_timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout)) < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> setsockopt(SO_RCVTIMEO) < 0");
        return NULL;
        }

    memset(&server_template_addr, 0, sizeof(server_template_addr));
    server_template_addr.sin_family = AF_INET;
    server_template_addr.sin_port = htons((unsigned short int)BROADCAST_PORT);

    char *broadcast_address = NULL;

    for (unsigned short int attempt = 0; attempt < 3 && broadcast_address == NULL; attempt++)
        broadcast_address = calloc(INET_ADDRSTRLEN, sizeof(char));
    
    if (broadcast_address == NULL)
        {
        perror("rpse_broadcast_receiveBroadcast() --> broadcast_address == NULL");
        return NULL;
        }
    
    _rpse_broadcast_getBroadcastAddress(broadcast_address);
    server_template_addr.sin_addr.s_addr = inet_addr(broadcast_address);

    if (bind(sockfd, (struct sockaddr *)&server_template_addr, sizeof(server_template_addr)) < 0)
        {
        perror("rpse_broadcast_receiveBroadcast() --> bind()");
        return NULL;
        }

    char *current_buffer = NULL;
    for (unsigned short int attempt = 0; attempt < 3 && current_buffer == NULL; attempt++)
        current_buffer = calloc(RECEIVER_BUFFER_SIZE, sizeof(char));

    if (current_buffer == NULL)
        {
        perror("rpse_broadcast_receiveBroadcast() --> current_buffer == NULL");
        return NULL;
        }
    
    string_dll_node_t *head = NULL;

    char *host_ip_address = rpse_broadcast_getIPAddress();
    if (host_ip_address == NULL)
    	{
	perror("rpse_broadcast_receiveBroadcast() --> host_ip_address == NULL");
	return NULL;
	}

    struct in_addr host_ip_addr_info;
    if (inet_pton(AF_INET, host_ip_address, &host_ip_addr_info) <= 0)
    	{
	perror("rpse_broadcast_receiveBroadcast() --> inet_pton() <= 0");
	return NULL;
	}

    free(host_ip_address);
    host_ip_address = NULL;

    struct sockaddr_in broadcaster_addr;
    broadcaster_addr.sin_addr.s_addr = 0; /* So no unitialised value will be used in an if statement */

    time_t start = time(NULL);

    /* Core part here */
    while (difftime(time(NULL), start) < RECEIVER_TIMEOUT)
        {
	socklen_t broadcaster_addr_size = sizeof(broadcaster_addr);
        memset(current_buffer, 0, RECEIVER_BUFFER_SIZE);

        int received_broadcast_len = recvfrom(sockfd, current_buffer, RECEIVER_BUFFER_SIZE, 0, (struct sockaddr *)&broadcaster_addr, &broadcaster_addr_size);

	if (broadcaster_addr.sin_addr.s_addr == host_ip_addr_info.s_addr)
	    continue;
	else if (received_broadcast_len == EAGAIN || received_broadcast_len == EWOULDBLOCK)
            continue;
        else if (received_broadcast_len <= 0)
            continue;

        else if (received_broadcast_len > 0 && current_buffer != NULL && (size_t)received_broadcast_len <= strlen(current_buffer))
	    {
	    perror("// DEBUG // rpse_broadcast_receiveBroadcast --> recvfrom()");
	    current_buffer[received_broadcast_len] = '\0';
	    if (head == NULL)
		    head = rpse_dll_createStringDLL(current_buffer);
	    else
		    rpse_dll_insertAtStringDLLEnd(&head, current_buffer);
	    }
            
        }
    /* We get rid of the nonce from each of the messages and decrypt them */
    string_dll_node_t *current_node;
    if (head == NULL)
	    current_node = NULL;
    else
	    current_node = head;
    broadcast_data_t *current_broadcast_data = calloc(1, sizeof(broadcast_data_t));
    if (current_broadcast_data == NULL)
        {
        perror("rpse_broadcast_receiveBroadcast() --> current_broadcast_data == NULL");
        return NULL;
        }
    
    if (head != NULL)
    	rpse_dll_deleteStringDLLDuplicateNodes(&head);

    while (current_node != NULL && current_broadcast_data != NULL)
        {
    	memset(current_broadcast_data->encrypted_message, 0, sizeof(current_broadcast_data->encrypted_message));
        if (current_node->data != NULL)
            {
            char *nonce_position = strstr(current_node->data, "/nonce=");
	    if (nonce_position == NULL)
	    	{
		perror("rpse_broadcast_receiveBroadcast() --> nonce_position == NULL");
	        if (current_node->next == NULL)
		    current_node = NULL;
		else
		    current_node = current_node->next;
		continue;
		}
	    size_t encrypted_data_size = nonce_position - current_node->data;
            memcpy(current_broadcast_data->encrypted_message, current_node->data, encrypted_data_size);
	    current_broadcast_data->encrypted_message[strlen(current_broadcast_data->encrypted_message)] = '\0';
	    nonce_position += strlen("/nonce=");
            memcpy(current_broadcast_data->nonce, nonce_position, strlen(nonce_position));
	    nonce_position = NULL;
            memset(current_node->data, 0, strlen(current_node->data) + 1);
            crypto_stream_chacha20_xor((unsigned char *)current_node->data, (const unsigned char *)current_broadcast_data->encrypted_message, 
                                                                strlen(current_broadcast_data->encrypted_message) + 1,
                                                                (const unsigned char *)current_broadcast_data->nonce, (const unsigned char *)BROADCAST_CHACHA20_ENCRYPTION_KEY);
	    const char *INVALID_PATTERN = "^\d+$";
	    regex_t invalid_pattern_regex;
	    if (regcomp(&invalid_pattern_regex, INVALID_PATTERN, REG_EXTENDED))
	    	{
		perror("rpse_broadcast_receiveBroadcast() --> regcomp() == EXIT_FAILURE");
		return NULL;
		}
	    if (current_node != NULL && current_node->data != NULL && strlen(current_node->data) > 1)
		{
	        while (
		       !regexec(&invalid_pattern_regex, current_node->data, 0, NULL, 0) || \
		       current_node->data[strlen(current_node->data) - 1] == 'n' \
		      ) /* these just appear all the time */
		      head->data[strlen(current_node->data) - 1] = '\0';
		regfree(&invalid_pattern_regex);
		}
	    }
        else
            {
            current_node = NULL;
            continue;
            }
        
        if (current_node->next == NULL)
            current_node = NULL;
        else
            current_node = current_node->next;
        }
    
    /* This is done this way beacause why would a client that wants to join a server see other clients and vice versa? */
    if (head != NULL)
	{
	if (BROADCAST_DATA->user_type == CLIENT_USER_TYPE)
	    {
	    if (_rpse_broadcast_verifyAndTrimDLLStructure(&head, SERVER_USER_TYPE, BROADCAST_DATA->username) == EXIT_FAILURE)
	    	{
		perror("rpse_broadcast_receiveBroadcast() --> SERVER_USER_TYPE --> _rpse_broadcast_verifyAndTrimDLLStructure() == EXIT_FAILURE");
		return NULL;
		}
	    }
    	    else if (BROADCAST_DATA->user_type == SERVER_USER_TYPE)
		{
	        if (_rpse_broadcast_verifyAndTrimDLLStructure(&head, CLIENT_USER_TYPE, BROADCAST_DATA->username) == EXIT_FAILURE)
                    {
		    perror("rpse_broadcast_receiveBroadcast() --> CLIENT_USER_TYPE --> _rpse_broadcast_verifyAndTrimDLLStructure() == EXIT_FAILURE");
		    return NULL;
		    }
		}
    	    else
	        {
                perror("rpse_broadcast_receiveBroadcast() --> BROADCAST_DATA->user_type INVALID FOR VERIFICATION");
	        return NULL;
	        }
	}
	    
    free(current_broadcast_data);
    current_broadcast_data = NULL;

    free(current_buffer);
    current_buffer = NULL;

    free(broadcast_address);
    broadcast_address = NULL;

    close(sockfd);
    return head;
}
