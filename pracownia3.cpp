#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <queue>

#define MEM 64000

ucontext_t T1, T2,Main;
ucontext_t a;
unsigned int indexOfCurrentContext = 0;

// std::queue<ucontext_t*> ready_queue;

void task_create(void (*f)(), ucontext_t a, std::queue<ucontext_t*> queue)
{
	getcontext(&a);
	makecontext(&a, f, 0);
	queue.push(&a);
}


void schedule(std::queue<ucontext_t*> q)
{
	ucontext_t c1 = *q.front();
	q.pop();
	q.push(&c1);
	ucontext_t c2 = *q.front();
	q.pop();
	q.push(&c2);
	if (indexOfCurrentContext == q.size())
	{
		swapcontext(&c1, &c2);
		indexOfCurrentContext = 0;
	}
	else
	{
		swapcontext(&queue[indexOfCurrentContext], &queue[indexOfCurrentContext+1]);
		indexOfCurrentContext++;
	}
}

int fn1()
{
	printf("this is from 1\n");
	setcontext(&Main);
}
void fn2()
{
	printf("this is from 2\n");
	setcontext(&a);
	printf("finished 1\n");
}
void start()
{
	getcontext(&a);
	a.uc_link=0;
	a.uc_stack.ss_sp=malloc(MEM);
	a.uc_stack.ss_size=MEM;
	a.uc_stack.ss_flags=0;
	makecontext(&a, (void (*)())&fn1, 0);
}

int main()
{
	std::queue<ucontext_t*> queue;
	ucontext_t a;
	task_create(fn2, a, queue);
	
	/*struct sigaction act;
	act.sa_handler = schedule;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);*/
	


	/*start();
	printf("after start\n");
	getcontext(&Main);
	getcontext(&T1);
	T1.uc_link=0;
	T1.uc_stack.ss_sp=malloc(MEM);
	T1.uc_stack.ss_size=MEM;
	T1.uc_stack.ss_flags=0;
	makecontext(&T1, (void (*)())&fn1, 0);
	swapcontext(&Main, &T1);
	usleep(1000);
	printf("after swapcontext\n");
	getcontext(&T2);
	T2.uc_link=0;
	T2.uc_stack.ss_sp=malloc(MEM);
	T2.uc_stack.ss_size=MEM;
	T2.uc_stack.ss_flags=0;
	makecontext(&T2, (void (*)())&fn2, 0);
	swapcontext(&Main, &T2);
	printf("completed\n");
	exit(0);*/
}