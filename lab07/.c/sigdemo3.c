#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int count = 0;
void f(int);

int main()
{
	signal( SIGINT, SIG_IGN );
	signal( SIGINT, f );

	printf("you can't stop me!\n");
	while(1)
	{
		sleep(1);
		printf("haha\n");
		count++;
	}
	return 0;
}

void f(int signum)
{
	printf("Currently elapsed time: %d sec(s)\n", count);
}

