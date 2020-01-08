
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  Record Eva = {"Eva Rosalene", 'F', 25};
  Record Damen = {"Damen Maughan", 'F', 20};
  Record Hatsune = {"Hatsune Miku", 'F', 13};
  Record RinK = {"Rin Kagamine", 'M', 13};
  Record Rin = {"Rin", 'F', 16};

  Hash_Table table = init_table();

  put_record( &Eva, table);
  put_record( &Damen, table);
  put_record( &Hatsune, table);
  put_record( &RinK, table);
  put_record( &Rin, table);

  dump_table(table);

  puts("Removing Hatsune Miku, then trying to look her up.");
  delete_key("Hatsune Miku", table);
  dump_table(table);

  puts("Looking at Eva.");
  print_record("Eva Rosalene", table);
  puts("Lets overwrite the existing Eva.");
  Record Eva2 = {"Eva Rosalene", 'F', 26};
  put_record( &Eva2, table);
  print_record("Eva Rosalene", table);
  dump_table(table);

  free(table);

  return 0;
}
