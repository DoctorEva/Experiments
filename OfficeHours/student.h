#ifndef STUDENT_H
#define STUDENT_H

#include <unistd.h>
#include <pthread.h>
#include "office.h"

enum Class{ ALG, CALC, PHYS };

typedef struct
{
  int arrival_time;
  int question_time;
  int class;
  char* name;
  pthread_t thread;
  Student_node* myNode;
} Student;

typedef struct _student_args
{
  Office* off;
  Student* std;
} Student_args;

void* student_thread(void* student_args);

void enter_office(Student* Student, Office* Office);
void leave_office(Student* Student, Office* Office);

void ask_questions(Student* Student);

#endif
