/*
  Rules:
    At most 3 students can be in the office at the same time.
    All students in the office must be in the same class.
    After helping X students from one class A, students from the other class B
      shall be given a chance to enter the office. No new students from class A
      may enter the office during this time.
    If no students are in the office, a student from either class may enter.
    After Y number of students, the professor will take a break.
*/
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>

#include "office.h"
#include "student.h"

char* statemap[] = { "Empty", "Full", "Breaktime", "Allowing"};
char* classmap[] = { "Algebra", "Calculus", "General Physics"};

void monitor( Office* o )
{
  initscr();
  puts("Starting monitor");
  int i = 0;
  while( 1 )
  {
    //lock_office(o);
    clear();
    printw("Number of students: %d\n", o->num_students);
    printw("Current class being seen: %s\n", classmap[o->current_class]);
    printw("Students since break: %d\n", o->students_since_break);
    printw("State: %s\n", statemap[o->state]);
    printw("Time: %d\n", i++);

    printw(" CURRENTLY IN OFFICE \n");
    printw( "Name:\tArrival:\tQuestion:\tClass:\n");
    Data_node* current = list_front(o->cur_students);
    while(current)
    {
      Student* s = (Student*) current->value_ptr;
      printw("%s\t\t%d\t\t%d\t%s\n", s->name, s->arrival_time, s->question_time, classmap[s->class]);
      current = current->next;
    }

    printw(" WAITING FOR ADMITTANCE \n");
    printw( "Name:\tArrival:\tQuestion:\tClass:\n");
    current = list_front(o->students);
    while(current)
    {
      Student* s = (Student*) current->value_ptr;
      printw("%s\t\t%d\t\t%d\t%s\n", s->name, s->arrival_time, s->question_time, classmap[s->class]);
      current = current->next;
    }
    //unlock_office(o);
    refresh();
    sleep(1);

  }
  endwin();
}

int main( int argc, char** argv )
{
  Office* Office = init_office("students.txt");

  pthread_t off;
  pthread_create(&off, NULL, office_thread, Office);

  monitor( Office );
  //unlock_office(Office);


}
