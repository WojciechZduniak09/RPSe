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

#include "../include/rpsecore-dll.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/*
======================
MISCELLANEOUS FUNCTION
======================
*/


/* Returns -1 for failures */
int
rpse_dll_getStringDLLNodeCount(string_dll_node_t **head)
{
	if (head == NULL)
		{
		perror("rpse_dll_getStringDLLNodeCount() --> head == NULL");
		return -1;
		}
		
	if ((*head) == NULL)
		{
		perror("rpse_dll_getStringDLLNodeCount() --> (*head) == NULL");
		return -1;
		}
	if ((*head)->prev != NULL)
		{
		perror("rpse_dll_getStringDLLNodeCount() --> (*head)->prev != NULL");
		return -1;
		}
	
	string_dll_node_t *current_node = *head;
	unsigned int position = 0;

	while (current_node != NULL)
		{
		current_node = current_node->next;
		position++;
		}

	current_node = NULL;

	return position;
}

/*
===============
CREATE FUNCTION
===============
*/


string_dll_node_t *
rpse_dll_createStringDLL(const char *DATA)
{
	if (DATA == NULL)
		{
		perror("rpse_dll_createStringDLL() --> DATA == NULL");
		return NULL;
		}

    string_dll_node_t *new_node = NULL;

    for (unsigned short int attempt = 0; attempt < 3 && new_node == NULL; attempt++)
        new_node = calloc(1, sizeof(string_dll_node_t));

    if (new_node == NULL)
		{
		perror("rpse_dll_createStringDLL() --> new_node == NULL");
		return NULL;
		}

       
    new_node->prev = NULL;

    const size_t NCHAR = strlen(DATA) + 1;
    new_node->data = NULL;

    for (unsigned short int attempt = 0; attempt < 3 && new_node->data == NULL; attempt++)
        new_node->data = calloc(NCHAR, sizeof(char));

    if (new_node->data == NULL)
        {
        perror("rpse_dll_createStringDLL() --> new_node->data == NULL");
	return NULL;
	}
	
    strncpy(new_node->data, DATA, strlen(DATA)+1);
    
    new_node->next = NULL;

    return new_node;
}

/*
===============
INSERT FUNCTION
===============
*/

unsigned short int
rpse_dll_insertAtStringDLLEnd(string_dll_node_t **head, const char *DATA)
{
	if (head == NULL)
		{
		perror("rpse_dll_insertAtStringDLLEnd() --> head == NULL");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("rpse_dll_insertAtStringDLLEnd() --> (*head) == NULL");
		return EXIT_FAILURE;
		}

	else if ((*head)->prev != NULL)
		{
		perror("rpse_dll_insertAtStringDLLEnd() --> (*head)->prev != NULL");
		return EXIT_FAILURE;
		}
	else if (DATA == NULL)
		{
		perror("rpse_dll_insertAtStringDLLEnd() --> DATA == NULL");
		return EXIT_FAILURE;
		}
	
    string_dll_node_t *new_node = rpse_dll_createStringDLL(DATA);

    string_dll_node_t *tail = (*head);
    while (tail->next != NULL)
        tail = tail->next;

    tail->next = new_node;
    new_node->prev = tail;

    tail = NULL;
    return EXIT_SUCCESS;
}

/*
================
DELETE FUNCTIONS
================
*/


unsigned short int
rpse_dll_deleteAtDLLStringPosition(string_dll_node_t **head, const unsigned int DELETION_POSITION)
{
	if (head == NULL)
		{
		perror("rpse_dll_deleteAtDLLStringPosition() --> head == NULL");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("rpse_dll_deleteAtDLLStringPosition() --> (*head) == NULL");
		return EXIT_FAILURE;
		}

	else if ((*head)->prev != NULL)
		{
		perror("rpse_dll_deleteAtDLLStringPosition() --> (*head)->prev == NULL");
		return EXIT_FAILURE;
		}
	else if ((*head)->data == NULL)
		{
		perror("rpse_dll_deleteAtDLLStringPosition() --> (*head)->data == NULL");
		return EXIT_FAILURE;
		}
	
	string_dll_node_t *tmp = *head;
	
	if (DELETION_POSITION == 1)
		{
		if ((*head)->next == NULL)
			rpse_dll_deleteStringDLL(head);

		else 
			{
			tmp = (*head)->next;
			free(tmp->prev->data);
			tmp->prev->data = NULL;
			free(tmp->prev);
			tmp->prev = NULL;
			(*head) = tmp;
			}
		
		tmp = NULL;
		return EXIT_SUCCESS;
		}

	for (unsigned int position = 1; tmp != NULL && position < DELETION_POSITION; position++)
		tmp = tmp->next;

	if (tmp == NULL)
		{
		perror("rpse_dll_deleteAtDLLStringPosition() --> tmp == NULL");
		return EXIT_FAILURE;
		}
	if (tmp->next != NULL)
		tmp->next->prev = tmp->prev;

	if (tmp->prev != NULL)
		tmp->prev->next = tmp->next;
	
	if (tmp != NULL)
		{
		if (tmp->data != NULL)
			free(tmp->data);
		tmp->data = NULL;
		
		free(tmp);
		tmp = NULL;
		}

	return EXIT_SUCCESS;
}

unsigned short int
rpse_dll_deleteStringDLLDuplicateNodes(string_dll_node_t **head)
{
	if (head == NULL)
		/* Not much of a problem here */
		return EXIT_SUCCESS;
	
	else if ((*head) == NULL)
		/* Not much of a problem here either */
		return EXIT_SUCCESS;

	else if ((*head)->prev != NULL)
		{
		/* Big uh oh here */
		perror("rpse_dll_deleteStringDLLDuplicateNodes() --> (*head)->prev != NULL");
		return EXIT_FAILURE;
		}

	string_dll_node_t *current_node = (*head);
	string_dll_node_t *next_node = current_node->next;
	int current_element_num = 1;

	while (next_node != NULL && next_node->data != NULL)
		{
		if (strcmp(current_node->data, next_node->data) == EXIT_SUCCESS)
			{
			if (rpse_dll_deleteAtDLLStringPosition(head, current_element_num) == EXIT_FAILURE)
				{
				perror("rpse_dll_deleteStringDLLDuplicateNodes() --> rpse_dll_deleteAtStringDLLPosition == EXIT_FAILURE");
				return EXIT_FAILURE;
				}
			}
		else
			{
			current_element_num++;
			current_node = next_node;
			next_node = current_node->next;
			}
		}
	
	current_node = NULL;
	return EXIT_SUCCESS;
}

unsigned short int
rpse_dll_deleteStringDLL(string_dll_node_t **head)
{
	if (head == NULL)
		{
		perror("rpse_dll_deleteStringDLL() --> head == NULL");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("rpse_dll_deleteStringDLL() --> (*head) == NULL");
		return EXIT_FAILURE;
		}
	
	else if ((*head)->prev != NULL)
		{
		perror("rpse_dll_deleteStringDLL() --> (*head)->prev != NULL");
		return EXIT_FAILURE;
		}

	if ((*head)->next == NULL)
		{
		free((*head)->data);
		(*head)->data = NULL;
		free((*head));
		(*head) = NULL;
		return EXIT_SUCCESS;
		}

	string_dll_node_t *current_node = (*head);
	string_dll_node_t *next_node = NULL;
	
	while (current_node != NULL)
		{
		next_node = current_node->next;
		free(current_node->data);
		current_node->data = NULL;

		free(current_node);
		current_node = next_node;
		}

	current_node = NULL;
	next_node = NULL;

	(*head) = NULL;

	return EXIT_SUCCESS;
}
