#include "read_file.h"

Data_node* read_file(char* filename)
{
  Data_node* ret = NULL;

  FILE* input;
  input = fopen(filename, "r");

  if(!input)
  {
    printf("read_file: Failed to open %s\n", filename);
    return NULL;
  }

  while( !feof(input) )
  {
    char* lineptr = NULL;
    size_t n = 0;
    getline(&lineptr, &n, input);
    ret = append_node( lineptr, 0, ret);
  }

  fclose(input);
  return ret;
}

Data_node* remove_comments_and_empty_lines(Data_node* lines, char comment_char)
{
  Data_node* current = list_front(lines);
  Data_node* ret = current;

  while(current)
  {
    Data_node* next = current->next;
    char* string = (char*) current->value_ptr;

    if( string[0] == '\n' || string[0] == comment_char )
    {
      free(string);
      ret = delete_node(current);
    }
    current = next;
  }

  return ret;
}
