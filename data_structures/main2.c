#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int hashcode(char* key)
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

int main(int argc, char** argv)
{
  Hash_Table table = init_table(&hashcode);

  put_record( (Record){"Eva Rosalene",  'F', 25}, table);
  put_record( (Record){"Damen Maughan", 'F', 20}, table);
  put_record( (Record){"Hatsune Miku",  'F', 13}, table);
  put_record( (Record){"Rin Kagamine",  'M', 13}, table);
  put_record( (Record){"Rin",           'F', 16}, table);

  dump_table(table);

  puts("Removing Hatsune Miku, then trying to look her up.");
  delete_key("Hatsune Miku", table);
  dump_table(table);

  puts("Looking at Eva.");
  print_record("Eva Rosalene", table);
  puts("Lets overwrite the existing Eva.");
  put_record( (Record){"Eva Rosalene", 'F', 26}, table);
  print_record("Eva Rosalene", table);
  dump_table(table);

  free_table(table);

  return 0;
}
