#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define MEM 100000

unsigned int lengthOfArray = 0;
unsigned int indexOfCurrentContext = 0;

int count = 0;
ucontext_t* context_array[10000];

void task_create(void (*f)(), ucontext_t* a)
{
	getcontext(a);
	a->uc_link=0;
	a->uc_stack.ss_sp=malloc(MEM);
	a->uc_stack.ss_size=MEM;
	a->uc_stack.ss_flags=0;
	makecontext(a, f, 0);
	context_array[lengthOfArray] = a;
	lengthOfArray++;
	printf("%d\n", lengthOfArray);
}

void fn1()
{
	for (int i=0; i<1000; i++)
	{
		printf("this is %i element from 1 thread\n", i);
		usleep(2000);
	}
}

void fn2()
{
	for (int i=0; i<1000; i++)
	{
		printf("this is %i element from 2 thread\n", i);
		usleep(2000);
	}
}

void fn3()
{
	for (int i=0; i<1000; i++)
	{
		printf("this is %i element from 3 thread\n", i);
		usleep(2000);	
	}
}

void schedule(int sig)
{
	if (count == 0)
	{
		++count;
		printf("First signal!\n");
		setcontext(context_array[0]);
	}
	printf("signal occurred %d times\n", ++count);
	if (lengthOfArray>1)
	{
		getcontext(context_array[indexOfCurrentContext]);
		if (indexOfCurrentContext==lengthOfArray-1)
		{
			printf("Last element\n");
			indexOfCurrentContext=0;
			swapcontext(context_array[lengthOfArray-1], context_array[indexOfCurrentContext]);
		}
		else
		{
			printf("Another element\n");
			indexOfCurrentContext++;
			swapcontext(context_array[indexOfCurrentContext-1], context_array[indexOfCurrentContext]);
		}
	}
	else
	{
		printf("Only element\n");
		setcontext(context_array[0]);
	}
}

void init_threads()
{
	struct itimerval it;
	struct sigaction act, oact;
	act.sa_handler = schedule;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGPROF, &act, &oact);
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 10;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 10;
	setitimer(ITIMER_PROF, &it, NULL);
	for ( ; ; ) ;
}

int main()
{
	ucontext_t c1, c2, c3;
	task_create(fn1, &c1);
	task_create(fn2, &c2);
	task_create(fn3, &c3);
	init_threads();
}