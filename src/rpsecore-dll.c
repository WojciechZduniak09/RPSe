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
#include "../include/rpsecore-error.h"
#include <stdlib.h>
#include <string.h>

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
		perror("\"head == NULL\" while attempting to count nodes in a string DLL");
		return -1;
		}
		
	if ((*head) == NULL)
		{
		perror("\"(*head) == NULL\" while attempting to count nodes in a string DLL");
		return -1;
		}
	if ((*head)->prev != NULL)
		{
		perror("\"(*head)->prev != NULL\" while attempting to count nodes in a string DLL");
		rpse_error_blameDev();
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
		perror("\"data == NULL\" while attempting to create a string DLL");
		rpse_error_blameDev();
		return NULL;
		}

    string_dll_node_t *new_node = NULL;

	for (unsigned short int attempt = 0; attempt < 3 && new_node == NULL; attempt++)
		new_node = calloc(1, sizeof(string_dll_node_t));

    if (new_node == NULL)
		{
		perror("\"new_node == NULL\" while attempting to calloc() memory for it");
		rpse_error_errorMessage("attempting to calloc() memory for a doubly linked list node");
		return NULL;
		}

    new_node->prev = NULL;

	const size_t NCHAR = strlen(DATA) + 1;
	new_node->data = NULL;

	for (unsigned short int attempt = 0; attempt < 3 && new_node->data == NULL; attempt++)
		new_node->data = calloc(NCHAR, sizeof(char));

	if (new_node->data == NULL)
		{
		perror("\"new_node->data == NULL\" while attempting to calloc() memory for it");
		rpse_error_errorMessage("attempting to calloc() memory for a doubly linked list node's data");
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
		perror("\"head == NULL\" while attempting to insert data at end of a string DLL");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("\"(*head) == NULL\" while attempting to insert data at end of a string DLL");
		return EXIT_FAILURE;
		}

	else if ((*head)->prev != NULL)
		{
		perror("\"(*head)->prev != NULL\" while attempting to insert data at end of a string DLL");
		return EXIT_FAILURE;
		}
	
    string_dll_node_t *new_node = rpse_dll_createStringDLL(DATA);

    string_dll_node_t *tmp = *head;
    while (tmp->next != NULL)
        tmp = tmp->next;

    tmp->next = new_node;
    new_node->prev = tmp;

	tmp = NULL;
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
		perror("\"head == NULL\" while attempting to delete a string DLL node");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("\"(*head) == NULL\" while attempting to delete a string DLL node");
		return EXIT_FAILURE;
		}

	else if ((*head)->prev != NULL)
		{
		perror("\"(*head)->prev != NULL\" while attempting to delete a string DLL node");
		return EXIT_FAILURE;
		}
	else if ((*head)->data == NULL)
		{
		perror("\"(*head)->data != NULL\" while attempting to delete a string DLL node");
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
			(*head) = tmp;
			(*head)->prev = NULL;
			}

		tmp = NULL;
		return EXIT_SUCCESS;
		}

	for (unsigned int position = 1; tmp != NULL && position < DELETION_POSITION; position++)
		tmp = tmp->next;

	if (tmp == NULL)
		rpse_error_blameDev();
	
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
		{
		/* Not much of a problem here */
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		/* Not much of a problem here either */
		return EXIT_FAILURE;
		}

	else if ((*head)->prev != NULL)
		{
		/* Big uh oh here */
		perror("\"(*head)->prev != NULL\" while attempting to delete duplicate string DLL nodes");
		rpse_error_blameDev();
		return EXIT_FAILURE;
		}

	string_dll_node_t *current_head_dll_node = *head;
	string_dll_node_t *current_auxiliary_dll_node = rpse_dll_createStringDLL("Initialiser data for auxiliary DLL which cannot be used by other "
																			 "functions as it's longer than the Great Wall of China.");
	int current_head_dll_element_num = 1;

	bool element_deleted = false;

	while (current_head_dll_node != NULL)
		{
		if (current_head_dll_node->data == NULL)
			{
			rpse_dll_deleteAtDLLStringPosition(head, current_head_dll_element_num);
			element_deleted = true;
			current_head_dll_node = current_head_dll_node->next;
			}
		
		else
			{
			while (current_auxiliary_dll_node != NULL)
				{
				int max_str_size = (strlen(current_head_dll_node->data) > strlen(current_auxiliary_dll_node->data) ? 
									strlen(current_head_dll_node->data) + 1 : strlen(current_auxiliary_dll_node->data) + 1);
				
				if (strncmp(current_head_dll_node->data, current_auxiliary_dll_node->data, max_str_size) == 0)
				/* 0 means that they're equal */
					{
					rpse_dll_deleteAtDLLStringPosition(head, current_head_dll_element_num);
					element_deleted = true;
					}

				if (element_deleted)
					{
					current_head_dll_element_num++;
					element_deleted = false;
					}
				
				current_auxiliary_dll_node = current_auxiliary_dll_node->next;
				}
			}

		if (element_deleted)
			{
			current_head_dll_element_num++;
			element_deleted = false;
			}
		
		current_head_dll_node = current_head_dll_node->next;
		}

	if (rpse_dll_deleteStringDLL(&current_auxiliary_dll_node) == EXIT_FAILURE)
		{
		perror("Failure while attempting to delete \"current_auxiliary_dll_node\"");
		return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}


unsigned short int
rpse_dll_deleteStringDLL(string_dll_node_t **head)
{
	if (head == NULL)
		{
		perror("\"head == NULL\" while attempting to delete a string DLL");
		return EXIT_FAILURE;
		}
	
	else if ((*head) == NULL)
		{
		perror("\"(*head) == NULL\" while attempting to delete a string DLL");
		return EXIT_FAILURE;
		}
	
	else if ((*head)->prev != NULL)
		{
		perror("\"(*head)->prev != NULL\" while attempting to delete a string DLL");
		rpse_error_blameDev();
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
