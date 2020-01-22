#include "office.h"
#include "student.h"
#include <string.h>

Office* init_office(char* filename)
{
  Office* ret = malloc( sizeof(Office) );
  memset(ret, 0, sizeof(Office));

  pthread_mutex_init(&ret->mutex, NULL);
  pthread_mutex_lock(&ret->mutex);

  Data_node* file = read_file(filename);
  file = remove_comments_and_empty_lines(file, '#');

  while(file)
  {
    char* text = (char*) file->value_ptr;
    Student s = { 0, 0, 0, "Default"};
    s.name = strdup(text);
    sscanf( text, " %*s %d %d %d \n", &s.arrival_time, &s.question_time, &s.class);
    sscanf( s.name, " %s %*d %*d %*d \n", s.name);

    Student* std = malloc( sizeof(Student) );
    memcpy( std, &s, sizeof(Student) );

    Student_args* args = malloc (sizeof(Student_args));
    args->off = ret;
    args->std = std;

    pthread_create( &std->thread, NULL, student_thread, args);
    free(text);
    file = delete_node(file);
  }

  return ret;
}

void* office_thread(void* office)
{
  Office* Off = (Office*) office;
  while(1)
  { // TODO - Replace with cohen sutherland style.
    lock_office(Off);
    Off->state = ALLOW;
    if ( Off->num_students == 0 )                { Off->state = EMPTY; }
    if ( Off->num_students >= MAX_CAP)           { Off->state = FULL; }
    if ( Off->students_since_break >= BREAK_AMT) { Off->state = BREAK; }
    unlock_office(Off);
  }
}

void lock_office(Office* Off )
{
  pthread_mutex_lock(&Off->mutex);
}
void unlock_office(Office* Off )
{
  pthread_mutex_unlock(&Off->mutex);
}
