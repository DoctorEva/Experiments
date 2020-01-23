#include "office.h"
#include "student.h"
#include <string.h>

Office* init_office(char* filename)
{
  Office* ret = malloc( sizeof(Office) );
  memset(ret, 0, sizeof(Office));

  int i;
  for(i=0; i<2; i++)
  {
    pthread_mutex_init(&ret->mutex[i], NULL);
    //locker(ret, i);
  }

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
    locker(Off, STATE);
    update_state(Off);
    if( Off->state == BREAK )
    {
      unlocker(Off, STATE);
      while( Off->num_students );
      locker(Off, STATE);
      Off->students_since_break = 0;
      sleep(10);
    }
    unlocker(Off, STATE);
  }
}

void update_state( Office* Off )
{
  Off->state = ALLOW;
  if ( Off->num_students == 0 )                { Off->state = EMPTY; }
  if ( Off->num_students >= MAX_CAP)           { Off->state = FULL; }
  if ( Off->students_since_break >= BREAK_AMT) { Off->state = BREAK; }
}

//_________________________________________
void locker(Office* Off, int index )
{
  pthread_mutex_lock(&Off->mutex[index]);
}
void unlocker(Office* Off, int index )
{
  pthread_mutex_unlock(&Off->mutex[index]);
}
