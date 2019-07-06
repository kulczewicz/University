#include <pthread.h>
#include <stdio.h>

using namespace std;

void *SubThread(void *numofthread) 
{
   long num = (long)numofthread;
   printf("Hi everyone! I am the %ldth thread\n", num);
   pthread_exit(NULL);
}

void *FirstThread(void*) 
{
   printf("Well, hello, World! Wait a sec, I'll create new threads...");
   pthread_t threads[7];
   
   for (long i = 1; i <= 7; i++)
   {
      printf("Creating thread number %ld\n", i);
      pthread_create(&threads[i], NULL, SubThread, (void *)i);
      pthread_join(threads[i], NULL);
   }
   printf("Threads were executed\n");
   pthread_exit(NULL);
}

int main () 
{
   pthread_t mainthread;
   printf("Creating main thread:");
   pthread_create(&mainthread, NULL, FirstThread , NULL);
   pthread_exit(NULL);
   return 0;
}