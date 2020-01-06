#ifndef LIST_H
#define LIST_H

typedef struct _Data_node
{
  void* value_ptr; // Should be cast to ptr of whatever struct we want.
  int typeID;
  struct _Data_node* next;
  struct _Data_node* previous;
} Data_node;

Data_node* append_node(void* value_ptr, int typeID, Data_node* list);
Data_node* delete_node(Data_node* target);

Data_node* list_back(Data_node* existing_node);
Data_node* list_front(Data_node* existing_node);

#endif
