#ifndef READ_FILE
#define READ_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

Data_node* read_file(char* filename);

Data_node* remove_comments_and_empty_lines(Data_node* lines, char comment_char);

// This is a typical comment.

#endif
