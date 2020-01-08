#include "hash_table.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*! _search_key() returns the node containing the record matching <key>
    PARAMS: The key to be retreived, and the hash table its in.
    RETURNS: The address of the Data_node containing the matching record
*/
Data_node* _search_key( char* key, Hash_Table t)
{
  int hcode = (*t._hashcode_func)(key);
  Data_node* current = list_front(t.table[hcode]);
  while(current)
  {
    char* stored_key = ((Record*) current->value_ptr)->name;
    if(strcmp(stored_key, key) == 0)
    {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// ___________________________________
/*! init_table() creates a new table and sets its hashcode function.
    PARAMS: Address of the function to be used to calculate hashcodes.
    RETURNS: A newly allocated Hash table.
*/
Hash_Table init_table( int(*hash_calc)(char*) )
{
  Hash_Table t;
  t._hashcode_func = hash_calc;
  t.table = (Data_node**) calloc(HASH_RANGE, sizeof(Data_node*));
  return t;
}

/*! put_record() adds a new record to the hash table, replacing the entry
      with the same name if it already exists.
    PARAMS: The record to be added, and the hash table to put it in.
    RETURNS: None.
*/
void put_record( Record entry, Hash_Table t)
{
  // Calc hcode for the entry.
  int hcode = (*t._hashcode_func)(entry.name);
  // See if the entry already exists, if so delete it.
  delete_key(entry.name, t);
  // Add the record to the hash table.
  Record* copy = (Record*) malloc (sizeof(Record));
  memcpy(copy, &entry, sizeof(Record));
  t.table[hcode] = append_node(copy, 0, t.table[hcode]);
}

/*! get_record() returns the Record matching <key>
    PARAMS: The key to be retrived, and the table to retrive it from
    RETURNS: Address of the record matching <key>, or NULL if it isnt found.
*/
Record* get_record( char* key, Hash_Table table)
{
  Data_node* record_holder = _search_key(key, table);
  if(record_holder)
  {
    return (Record*) record_holder->value_ptr;
  }
  return NULL;
}

/*! delete_key() removes the associated record from the table,
      freeing it and removing the node it was on.
    PARAMS: The key to remove, and the table to free it from.
    RETURNS: None
*/
void delete_key( char* key, Hash_Table t)
{
  Data_node* record_holder = _search_key(key, t);
  if(record_holder)
  {
    int hcode = (*t._hashcode_func)(key);
    free(record_holder->value_ptr);
    t.table[hcode] = delete_node(record_holder);
  }
}

//___________________________________

/*! print_record() prints out a single record in detail
    PARAMS: The key to be retrived, and the table its in.
    RETURNS: None
*/
void print_record( char* key, Hash_Table table)
{
  Record* r = get_record(key, table);
  if(r)
  {
    printf("Name: %s | Gender: %c | Age: %d\n", r->name, r->gender, r->age);
  }
  else
  {
    printf("'%s' not found.\n", key);
  }
}

/*! dump_table() prints out the tables' keys, at the
    index that they reside at and all collisions.
    PARAMS: The table to be printed.
    RETURNS: None.
*/
void dump_table(Hash_Table t)
{
  puts("Dumping Hash Table members");
  int i;
  for(i=0; i<HASH_RANGE; i++)
  {
    Data_node* current = list_front(t.table[i]);
    printf("%d/", i);
    while(current)
    {
      Record* record = (Record*) current->value_ptr;
      printf(" -> %s", record->name);
      current = current->next;
    }
    printf(" :: END\n");
  }
}

//___________________________________
/*! free_table() frees all nodes and their records, then frees the table.
    PARAMS: The table to be freed.
*/
void free_table(Hash_Table t)
{
  int i;
  for(i=0; i<HASH_RANGE; i++)
  {
    Data_node* current = list_front(t.table[i]);
    while(current)
    {
      free( current->value_ptr );
      current = delete_node( current );
    }
  }
  free(t.table);
}
