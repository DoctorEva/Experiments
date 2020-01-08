// Compilation line: gcc main.c linked_list.c

#include "linked_list.h"
#include <stdio.h>

enum typeIDs{ INT, STRING };

void dump_list(Data_node* list)
{
  Data_node* start = list_front(list);
  Data_node* current = start;
  while(current != NULL)
  {
    void* val = current->value_ptr;
    switch(current->typeID)
    {
      case INT:
        printf("-> %d ", *(int*)val);
        break;
      case STRING:
        printf("-> %s ", (char*)val);
        break;
      default:
        puts("TypeID not recognized, what is this node carrying?");
    }
    current = current->next;
  }
  puts("::END");
}

int main(int argc, char** argv)
{
  int x = 23;
  int y = 525;
  int z = 666;
  char str[] = "Dr.Eva Rosalene Presents:";
  char str2[] = "A mixed type Doubly Linked List!";

  // Lets start a new list and add to it using append node.
  Data_node* list = append_node(&x, INT, NULL);
  append_node(&y, INT, list);
  append_node(&z, INT, list);

  // Lets print out the list members.
  dump_list(list);

  // Lets delete a node.
  list = delete_node(list_front(list));
  // We must save its result so we dont lose the list.
  append_node(&z, INT, list);
  append_node(&str, STRING, list);
  append_node(&str2, STRING, list);
  dump_list(list);

  // Lets clear the list.
  while(list)
  {
    list = delete_node(list);
  }
}
