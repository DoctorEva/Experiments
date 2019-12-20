#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>


typedef struct _jobs
{
  int input;
  int* output_buffer;
}Job;

typedef struct _thread_args
{
  std::queue<Job> joblist;
}thread_args;

void* Worker_work(void* _args)
{
  thread_args* args = (thread_args*) _args;
  puts("Worker starting to work");
  while(1)
  {
    if(!args->joblist.empty())
    {
      Job current = args->joblist.front();
      args->joblist.pop();

      *current.output_buffer = current.input * 100;
    }
  }
}

int main(int argc, char** argv)
{
  struct _thread_args args;

  pthread_t Worker;
  pthread_create(&Worker, NULL, Worker_work, (void*) &args);

  int outputs[3] = {0,0,0};
  Job newjob;

  newjob.input = 1;
  newjob.output_buffer = &outputs[0];
  args.joblist.push(newjob);
  while(!outputs[0]);

  newjob.input = 2;
  newjob.output_buffer = &outputs[1];
  args.joblist.push(newjob);
  while(!outputs[1]);

  newjob.input = 3;
  newjob.output_buffer = &outputs[2];
  args.joblist.push(newjob);
  while(!outputs[2]);

  printf("Outputs are %d, %d, %d\n", outputs[0], outputs[1], outputs[2]);
  puts("Program exiting");
  return 0;
}
