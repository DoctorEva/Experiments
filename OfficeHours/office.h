#ifndef OFFICE_H
#define OFFICE_H

#include <unistd.h>
#include <pthread.h>
#include "../data_structures/linked_list.h"
#include "../data_structures/read_file.h"

#define MAX_CAP 3
#define BREAK_AMT 10

typedef Data_node Student_node;

enum Office_States{ EMPTY, FULL, BREAK, ALLOW};
enum Mutexes{ STATE, OFFICE};

typedef struct _Office
{
  int num_students;
  int current_class;
  int students_since_break;
  Student_node* students;
  Student_node* cur_students;
  int state;
  pthread_mutex_t mutex[2];
} Office;

Office* init_office(char* filename);
void* office_thread(void* office);

void locker(Office* Off, int index );
void unlocker(Office* Off, int index );
void update_state( Office* Off );

//void take_break();

#endif
