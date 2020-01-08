#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

/*! list_back() finds the backmost node of the list.
    RETURNS: the address of the last node in the list.
    PARAMS: Address of any node in the list.
*/
Data_node* list_back(Data_node* existing_node)
{
  if(!existing_node)
    return NULL;

  Data_node* current = existing_node;
  while(current->next != NULL)
  {
    current = current->next;
  }
  return current;
}

/*! list_front() finds the frontmost node of the list.
    RETURNS: the address of the first node in the list.
    PARAMS: Address of any node in the list.
*/
Data_node* list_front(Data_node* existing_node)
{
  if(!existing_node)
    return NULL;

  Data_node* current = existing_node;
  while(current->previous != NULL)
  {
    current = current->previous;
  }
  return current;
}

/*! append_node() adds a new node to the end of an existing list,
      or begins a new list with the one node.
    RETURNS: the address of the newly added node.
    PARAMS: Address of an object or value,
            unique ID indicating the type of the value,
            Address of any node in the list or NULL to start a new list.
*/
Data_node* append_node(void* value_ptr, int typeID, Data_node* list)
{
  Data_node* end = list_back(list);
  Data_node* new_node = (Data_node*) malloc(sizeof(Data_node));

  new_node->next = NULL;
  new_node->value_ptr = value_ptr;
  new_node->typeID = typeID;
  new_node->previous = end;

  if(end)
  {
    end->next = new_node;
  }
  return new_node;
}

/*! delete_node() frees up a node and removes it from the linked list.
    RETURNS: the address of an adjacent node, or NULL if the list is now empty.
    PARAMS: Address of the node to delete.
    NOTE: Value should be freed first, if allocated on the heap, to prevent memory leaks.
*/
Data_node* delete_node(Data_node* target)
{
  if(!target)
  {
    puts("delete_node: Target is null.");
    return NULL;
  }

  Data_node* prev_node = target->previous;
  Data_node* next_node = target->next;
  free(target);

  if(next_node)
    next_node->previous = prev_node;

  if(prev_node)
   prev_node->next = next_node;

  return (prev_node) ? prev_node : next_node;
}

//___________________________________
