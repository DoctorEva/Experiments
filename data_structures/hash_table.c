#include "hash_table.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>

int _hashcode(char* key)
{
  // Note: Hashcode should be bounded between 0 and HASH_RANGE-1
  int ret = 0;
  if(strlen(key))
  {
    ret = key[0] % HASH_RANGE;
  }
  assert(ret < HASH_RANGE && ret >= 0);
  return ret;
}

Data_node* _search_key( char* key, Hash_Table table)
{
  int hcode = _hashcode(key);
  Data_node* current = list_front(table[hcode]);
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

Hash_Table init_table()
{
  return (Data_node**) calloc(HASH_RANGE, sizeof(Data_node*));
}

void put_record( Record* entry, Hash_Table table)
{
  // Calc hcode for the entry.
  int hcode = _hashcode(entry->name);
  // See if the entry already exists, if so delete it.
  delete_key(entry->name, table);
  // Add the record to the hash table.
  table[hcode] = append_node(entry, 0, table[hcode]);
}

Record* get_record( char* key, Hash_Table table)
{
  Data_node* record_holder = _search_key(key, table);
  if(record_holder)
  {
    return (Record*) record_holder->value_ptr;
  }
  return NULL;
}

void delete_key( char* key, Hash_Table table)
{
  Data_node* record_holder = _search_key(key, table);
  if(record_holder)
  {
    int hcode = _hashcode(key);
    table[hcode] = delete_node(record_holder);
  }
}

//___________________________________

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
void dump_table(Hash_Table table)
{
  puts("Dumping Hash Table members");
  int i;
  for(i=0; i<HASH_RANGE; i++)
  {
    Data_node* current = list_front(table[i]);
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
