#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "data_structures/linked_list.h"

enum jobIDs { EAT, LIVE, LOVE, DIE};

typedef struct eat_job
{
  int input;
  int output;
} Eat;

void eat(Eat* job)
{
  puts("Good food, good friends, good times!");
}

typedef struct live_job
{
  int input;
  int output;
} Live;

void live(Live* job)
{
  puts("Live kindly~");
}

typedef struct love_job
{
  int input;
  int output;
} Love;

void love(Love* job)
{
  puts("Love completely!");
}

Data_node* JobList = NULL;
pthread_mutex_t mtx;

void* Worker_work(void* _args)
{
  puts("Worker starting to work");
  int is_looping = 1;
  while(is_looping)
  {
    if(JobList)
    {
      void* val = JobList->value_ptr;
      switch(JobList->typeID)
      {
        case EAT:
          eat(val);
          break;
        case LIVE:
          live(val);
          break;
        case LOVE:
          love(val);
          break;
        case DIE:
          puts("Worker exiting.");
          is_looping = 0;
          break;
        default:
          puts("JobID not recognized.");
      }
      JobList = delete_node(JobList);
    }
  }
}

int main(int argc, char** argv)
{
  Eat Ea = {0,0};
  Live Li = {1,1};
  Love Lo = {2,2};

  pthread_t Worker;
  pthread_create(&Worker, NULL, Worker_work, NULL);

  while(1)
  {
  JobList = append_node(&Ea, EAT, JobList);
  JobList = append_node(&Li, LIVE, JobList);
  JobList = append_node(&Lo, LOVE, JobList);
  //JobList = append_node(NULL, DIE, JobList);
  }

  pthread_join(Worker, NULL);
  puts("Program exiting");
  return 0;
}
