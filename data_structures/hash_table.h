#ifndef HASH_H
#define HASH_H

#include "linked_list.h"
#define HASH_RANGE 10

typedef struct _Record
{
  // This value is used as a unique key, should be left in.
  char* name;
  // Add whatever members you want your record to have --
      // print_record() will need to be changed to show it.
  char gender;
  int age;
} Record;

typedef struct _Hash_Table
{
  Data_node** table;
  int (*_hashcode_func)(char*);
} Hash_Table;

Hash_Table init_table( int(*_hashcode_func)(char*) );

void put_record( Record entry, Hash_Table table);
void delete_key( char* key, Hash_Table table);

Record* get_record( char* key, Hash_Table table);

void print_record( char* key, Hash_Table table);
void dump_table(Hash_Table table);

void free_table(Hash_Table table);

#endif
