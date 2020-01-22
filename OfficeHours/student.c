#include "student.h"

void* student_thread(void* student_args)
{
  Student_args* args = (Student_args*) student_args;
  Student* Stud = (Student*) args->std;
  Office* off =   (Office*)  args->off;
  lock_office(off);
  sleep( Stud->arrival_time);

  off->students = append_node( Stud, 0, off->students);
  Stud->myNode = off->students;
  enter_office(Stud, off);
  ask_questions(Stud);
  leave_office(Stud, off);
  free(Stud);
  free(args);
}

void enter_office(Student* Student, Office* Office)
{
  // Conditions for entering.
  int is_waiting = 1;
  while( is_waiting )
  {
    if(Office->state == EMPTY)
    {
      is_waiting = 0;
    }
    else if(Office->state == ALLOW)
    {
      is_waiting = !(Office->current_class == Student->class);
    }
    else
    {
      sleep(0);
    }
  }


  Office->current_class = Student->class;
  Office->students = delete_node(Student->myNode);
  Office->cur_students = append_node( Student, 0, Office->cur_students);
  Student->myNode = Office->cur_students;
  Office->num_students++;
  Office->students_since_break++;
  unlock_office(Office);
}

void leave_office(Student* Student, Office* Office)
{
  Office->cur_students = delete_node(Student->myNode);
  Office->num_students--;
}

void ask_questions(Student* Student)
{
  while(Student->question_time)
  {
    sleep(1);
    Student->question_time--;
  }
}
