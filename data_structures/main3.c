#include <stdlib.h>

#include "read_file.h"
#include "linked_list.h"

int main(int argc, char** argv)
{
  Data_node* lines = read_file("read_file.h");
  lines = remove_comments_and_empty_lines(lines, '#');
  lines = list_front(lines);
  while(lines)
  {
    char* val = (char*) lines->value_ptr;
    printf("%s", val);
    free(val);
    lines = delete_node(lines);
  }

  return 0;
}
