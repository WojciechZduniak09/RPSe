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

#include "../include/rpsecore-dll.h"
#include "../include/rpsecore-error.h"
#include <stdlib.h>
#include <string.h>

/*
===============
CREATE FUNCTION
===============
*/


dll_node_t *
rpse_dll_createDLL(const char *DATA)
{
    dll_node_t *new_node = malloc(sizeof(dll_node_t));
    rpse_error_checkDLLNodeMalloc(new_node);

    new_node->prev = NULL;
    strncpy(new_node->data, DATA, strlen(DATA)+1);
    new_node->next = NULL;

    return new_node;
}

/*
===============
INSERT FUNCTION
===============
*/

void
rpse_dll_insertAtDLLEnd(dll_node_t **head, const char *DATA)
{
    dll_node_t *new_node = rpse_dll_createDLL(DATA);
    rpse_error_checkDLLNodePtrMalloc(head);

    dll_node_t *tmp = *head;
    while (tmp->next != NULL)
		{
        tmp = tmp->next;
    	}

    tmp->next = new_node;
    new_node->prev = tmp;

	tmp = NULL;
}

/*
================
DELETE FUNCTIONS
================
*/


void
rpse_dll_deleteAtDLLPostion(dll_node_t **head, const unsigned int DELTETION_POSITION)
{
	rpse_error_checkDLLNodePtrMalloc(head);
	dll_node_t *tmp = *head;
	
	if (DELTETION_POSITION == 1)
		{
		(*head) = (*head)->next;
		if ((*head) != NULL)
			(*head)->prev = NULL;
		free(tmp);
		}

	for (unsigned int position = 1; tmp != NULL && position < DELTETION_POSITION; position++)
		tmp = tmp->next;

	if (tmp == NULL)
		rpse_error_blameDev();
	
	if (tmp->next != NULL)
		tmp->next->prev = tmp->prev;

	if (tmp->prev != NULL)
		tmp->prev->next = tmp->next;
	
	free(tmp->data);
	tmp->data = NULL;

	free(tmp);
	tmp = NULL;
}

void 
rpse_dll_deleteDLLDuplicateNodes(dll_node_t **head)
{
	rpse_error_checkDLLNodePtrMalloc(head);
	dll_node_t *current = *head;
	dll_node_t *runner = current->next;

	while (current->next != NULL) 
		{
		runner = current->next;
		while (runner->next != NULL)
			{
			if (strcmp(runner->data, current->data) == 0)
				{
				dll_node_t* tmp = runner;
				
				if (runner->next!=NULL)
					runner->next->prev = runner->prev;

				if (runner->prev!=NULL)
					runner->prev->next = runner->next;
				
				runner = runner->next;
				free(tmp);
				tmp = NULL;

			}
			else
				runner = runner->next;
			current = current->next;
			}
		}
	
	current = NULL;
	runner = NULL;
}

void
rpse_dll_deleteDLL(dll_node_t **head)
{
	dll_node_t *current = *head;
	dll_node_t *next;

	while (current != NULL)
		{
		next = current->next;

		free(current->data);
		current->data = NULL;

		free(current);
		current = next;
		}
	
	current = NULL;
	next = NULL;
	*head = NULL;
}

/*
======================
MISCELLANEOUS FUNCTION
======================
*/

unsigned int
rpse_dll_getNodeCount(dll_node_t **head)
{
	dll_node_t *current = *head;
	unsigned int position = 1;

	while (current != NULL)
		{
		current = current->next;
		position++;
		}

	current = NULL;

	return position;
}